// Copyright (c) 2010-2014, The Video Segmentation Project
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the The Video Segmentation Project nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---

#include "base/base.h"
#include "base/base_impl.h"

#include <gflags/gflags.h>
#include <opencv2/highgui/highgui.hpp>

#include "segmentation/dense_segmentation.h"
#include "segmentation/region_segmentation.h"
#include "segmentation/segmentation_unit.h"
#include "video_framework/conversion_units.h"
#include "video_framework/flow_reader.h"
#include "video_framework/video_capture_unit.h"
#include "video_framework/video_reader_unit.h"
#include "video_framework/video_pipeline.h"
#include "video_framework/video_writer_unit.h"

DEFINE_bool(flow, true, "Determine if optical flow should be computed.");
DEFINE_string(input_file, "", "The input video file to segment. Use 'CAMERA' to read "
                              "from input camera. Adopt chunk_size and chunk_set_size "
                              "in that case to smaller values.");
DEFINE_bool(write_to_file, false, "If set write segmentation result to specified "
                                  "input_file + .pb");
DEFINE_bool(logging, false, "If set output various logging information.");
DEFINE_int32(downscale_min_size, 0, "If set > 0, downsamples input video to specifed "
                                    "dimension.");

namespace seg = segmentation;
namespace vf = video_framework;

int main(int argc, char** argv) {
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_logging) {
    FLAGS_logtostderr = 1;
  }

  if (FLAGS_input_file.empty()) {
    LOG(ERROR) << "Specified input file is empty.";
    return -1;
  }

  std::vector< std::unique_ptr<vf::VideoPipelineSource> > sources;
  std::vector< std::unique_ptr<vf::VideoPipelineSink> > sinks;

  std::unique_ptr<vf::VideoReaderUnit> reader;
  vf::VideoUnit* root = nullptr;
  // At every point, points to the latest unit added to the pipeline.
  vf::VideoUnit* input = nullptr;

  // Determine if flow is being used and if it is being read from file.
  std::string flow_file = FLAGS_input_file.substr(0, FLAGS_input_file.find_last_of(".")) + ".flow";
  bool use_flow_from_file = false;
  if (FLAGS_flow) {
    use_flow_from_file = base::FileExists(flow_file);
  }

  // First unit is the reader unit for the video.
  vf::VideoReaderOptions reader_options;
  reader.reset(new vf::VideoReaderUnit(reader_options, FLAGS_input_file));
  root = input = reader.get();

  // Tree:
  // sink ---> reader
  sinks.emplace_back(new vf::VideoPipelineSink());
  sinks.back()->AttachTo(input);
  vf::SourceRatePolicy srp;
  if (FLAGS_flow && !use_flow_from_file) {
    // Ensure we are not computing too many flow files.
    srp.respond_to_limit_rate = true;
    srp.rate_scale = 1.25f;
    srp.sink_max_queue_size = 10;
  }
  
  // Tree: 
  // source ---> sink ---> reader
  sources.emplace_back(new vf::VideoPipelineSource(sinks.back().get(), nullptr, srp));
  input = sources.back().get();

  // Add dense flow unit to pipeline.
  std::unique_ptr<vf::LuminanceUnit> lum_unit;
  std::unique_ptr<vf::DenseFlowUnit> dense_flow_unit;
  std::unique_ptr<vf::DenseFlowReaderUnit> dense_flow_reader;

  if (FLAGS_flow) {
    if (use_flow_from_file) {
      // Tree:
      // flow ---> source ---> sink ---> reader
      dense_flow_reader.reset(new vf::DenseFlowReaderUnit(vf::DenseFlowReaderOptions(), flow_file));
      dense_flow_reader->AttachTo(input);
      input = dense_flow_reader.get();
    }
    else {
      // Tree:
      // lum ---> source ---> sink ---> reader
      lum_unit.reset(new vf::LuminanceUnit(vf::LuminanceOptions()));
      lum_unit->AttachTo(input);
      vf::DenseFlowOptions flow_options;
      flow_options.flow_iterations = 10;
      flow_options.num_warps = 2;
      flow_options.video_out_stream_name = "";

      // Tree:
      // flow ---> lum ---> source ---> sink ---> reader
      dense_flow_unit.reset(new vf::DenseFlowUnit(flow_options));
      dense_flow_unit->AttachTo(lum_unit.get());
      input = dense_flow_unit.get();

      // Tree:
      // source ---> sink ---> flow ---> lum ---> source ---> sink ---> reader
      sinks.emplace_back(new vf::VideoPipelineSink());
      sinks.back()->AttachTo(input);
      sources.back()->SetMonitorSink(sinks.back().get());
      sources.emplace_back(new vf::VideoPipelineSource(sinks.back().get()));
      input = sources.back().get();
    }
  }

  // Add dense segmentation unit to pipeline.
  seg::DenseSegmentationUnitOptions dense_seg_unit_options;
  seg::DenseSegmentationOptions dense_seg_options;

  std::unique_ptr<seg::DenseSegmentationUnit> dense_segment(
      new seg::DenseSegmentationUnit(dense_seg_unit_options, &dense_seg_options));

  // Tree:
  // dense ---> source ---> sink ---> flow ---> lum --> source ---> sink ---> reader
  dense_segment->AttachTo(input);
  input = dense_segment.get();
  
  // Tree:
  // source ---> sink ---> dense ---> source ---> sink ---> flow ---> lum --> source ---> sink ---> reader
  sinks.emplace_back(new vf::VideoPipelineSink());
  sinks.back()->AttachTo(input);
  sources.emplace_back(new vf::VideoPipelineSource(sinks.back().get()));
  input = sources.back().get();

  // Add region segmentation unit to pipeline (merges regions previously
  // computed during dense segmentation).
  std::unique_ptr<seg::RegionSegmentationUnit> region_segment;
  seg::RegionSegmentationUnitOptions region_unit_options;

  region_segment.reset(new seg::RegionSegmentationUnit(region_unit_options, nullptr));
  region_segment->AttachTo(input);
  input = region_segment.get();

  // Tree:
  // source ---> sink ---> region ---> source ---> sink ---> dense ---> source ---> sink ---> flow ---> lum --> source ---> sink ---> reader
  sinks.emplace_back(new vf::VideoPipelineSink());
  sinks.back()->AttachTo(input);
  sources.emplace_back(new vf::VideoPipelineSource(sinks.back().get()));
  input = sources.back().get();

  // Add writer unit to pipeline if requested.
  std::unique_ptr<seg::SegmentationWriterUnit> seg_writer;
  if (FLAGS_write_to_file) {
    std::string out_file = FLAGS_input_file + ".pb";
    LOG(INFO) << "Writing result to file " << out_file;
    seg::SegmentationWriterUnitOptions writer_options;
    writer_options.filename = out_file;
    writer_options.remove_rasterization = true;
    seg_writer.reset(new seg::SegmentationWriterUnit(writer_options));
    seg_writer->AttachTo(input);
    input = seg_writer.get();
  }

  LOG(INFO) << "Tree layout:";
  root->PrintTree();

  if (!root->PrepareProcessing()) {
    LOG(ERROR) << "Setup failed.";
  }
  vf::VideoPipelineInvoker invoker;
  vf::RatePolicy pipeline_policy;

  // Setup rate policy with 20 fps max processing, startup of 2 frames (flow uses two),
  // and dynamic updates to pipeline every 30 frames.
  pipeline_policy.max_rate = 20;
  pipeline_policy.dynamic_rate = true;
  pipeline_policy.startup_frames = 10;
  pipeline_policy.update_interval = 1;
  pipeline_policy.queue_throttle_threshold = 10;
  // Guarantee that buffers never go empty in non-camera mode.
  pipeline_policy.dynamic_rate_scale = 1.1;

  // Start the threads.
  // First source is run rate limited.
  // root is the video reader unit.
  invoker.RunRootRateLimited(pipeline_policy, root);

  // Successively run all the units.
  for (int k = 0; k < sources.size() - 1; ++k) {
    invoker.RunPipelineSource(sources[k].get());
  }

  sources.back()->Run();

  invoker.WaitUntilPipelineFinished();

  LOG(INFO) << "__SEGMENTATION_FINISHED__";
  return 0;
}

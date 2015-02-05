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

#include <glog/logging.h>
#include "gflags/gflags.h"
#include "opencv2/opencv.hpp"

#include "base/base_impl.h"
#include "video_framework/video_pipeline.h"
#include "video_framework/video_reader_unit.h"
#include "video_framework/video_writer_unit.h"

DEFINE_string(input_file, "", "input video to save as image sequence");
DEFINE_string(output_file, "", "output video");
DEFINE_bool(logging, false, "enable logging");  

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  
  if (FLAGS_logging) {
    FLAGS_logtostderr = 1;
  }
  
  if (FLAGS_input_file.empty()) {
    LOG(ERROR) << "No input file provided.\n";
    LOG(ERROR) << "Usage: ./video_reader_writer --input_file=in.mov --output_file=ou.mov\n";
    return -1;
  }
  
  if (FLAGS_output_file.empty()) {
    LOG(ERROR) << "No output folder prvided.\n";
    LOG(ERROR) << "Usage: ./video_reader_writer --input_file=in.mov --output_file=ou.mov\n";
    return -1;
  }
  
  if (!base::FileExists(FLAGS_input_file)) {
    LOG(ERROR) << "Input file does not exist.\n";
    return -1;
  }
  
  if (base::FileExists(FLAGS_output_file)) {
    LOG(ERROR) << "Output file already exists.\n";
    return -1;
  }
  
  std::vector< std::unique_ptr<video_framework::VideoPipelineSource> > sources;
  std::vector< std::unique_ptr<video_framework::VideoPipelineSink> > sinks;
  video_framework::VideoUnit* root = nullptr;
  video_framework::VideoUnit* input = nullptr;
  
  std::unique_ptr<video_framework::VideoReaderUnit> 
          reader(new video_framework::VideoReaderUnit(video_framework::VideoReaderOptions(), FLAGS_input_file));
  
  root = input = reader.get();
  
  sinks.emplace_back(std::unique_ptr<video_framework::VideoPipelineSink>(new video_framework::VideoPipelineSink()));
  sinks.back()->AttachTo(input);
  
  sources.emplace_back(std::unique_ptr<video_framework::VideoPipelineSource>(
          new video_framework::VideoPipelineSource(sinks.back().get(),
          nullptr,
          video_framework::SourceRatePolicy())));
  
  input = sources.back().get();
  
  video_framework::VideoWriterOptions writer_options;
  std::unique_ptr<video_framework::VideoWriterUnit> video_writer(new video_framework::VideoWriterUnit(
          writer_options, FLAGS_output_file));
  video_writer->AttachTo(input);
  input = video_writer.get();
  
  root->PrintTree();
  
  if (!root->PrepareProcessing()) {
    LOG(ERROR) << "Setup failed.";
  }
  
  video_framework::VideoPipelineInvoker invoker;
  video_framework::RatePolicy pipeline_policy;

  pipeline_policy.max_rate = 20;
  pipeline_policy.dynamic_rate = true;
  pipeline_policy.startup_frames = 10;
  pipeline_policy.update_interval = 1;
  pipeline_policy.queue_throttle_threshold = 10;
  pipeline_policy.dynamic_rate_scale = 1.1;
  
  invoker.RunRootRateLimited(pipeline_policy, root);
  sources.back()->Run();
  invoker.WaitUntilPipelineFinished();
  
  return 0;
}
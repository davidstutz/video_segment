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

#include "retinex_pde_unit.h"

#include "limare2011-intrinsic-images/retinex_pde_lib_wrapper.h"
#include <opencv2/opencv.hpp>
#include "base/base_clock.h"

namespace retinex {
  using namespace video_framework;
  
  RetinexPDEUnit::RetinexPDEUnit(RetinexPDEOptions& options) : options_(options) {
    if (options_.threshold <= 0 || options_.threshold > 255) {
      LOG(ERROR) << "Threshold for Retinex PDE (provided by RetinexPDEOptions) has to be in the range (0, 255].";
    }
  }
  
  bool RetinexPDEUnit::OpenStreams(StreamSet* set) {
    video_stream_idx_ = FindStreamIdx(options_.video_stream_name, set);
    
    if (video_stream_idx_ < 0) {
      LOG(ERROR) << "Could not find stream!\n";
      return false;
    }
    
    const VideoStream& vid_stream = set->at(video_stream_idx_)->As<VideoStream>();
    
    frame_width_ = vid_stream.frame_width();
    frame_height_ = vid_stream.frame_height();
    frame_width_step_ = vid_stream.width_step();
    
    // Create the reflectance and shading streams:
    VideoStream* reflectance_stream = new VideoStream(frame_width_,
            frame_height_,
            frame_width_step_,
            vid_stream.fps(),
            vid_stream.pixel_format(),
            options_.reflectance_stream_name);
    VideoStream* shading_stream = new VideoStream(frame_width_,
            frame_height_,
            frame_width_step_,
            vid_stream.fps(),
            vid_stream.pixel_format(),
            options_.shading_stream_name);
    
    set->push_back(std::shared_ptr<VideoStream>(reflectance_stream));
    set->push_back(std::shared_ptr<VideoStream>(shading_stream));
  
    return true;
  }
  
  void RetinexPDEUnit::ProcessFrame(FrameSetPtr input, std::list<FrameSetPtr>* output) {
    const VideoFrame* frame = input->at(video_stream_idx_)->AsPtr<VideoFrame>();
    
    cv::Mat frame_mat;
    frame->MatView(&frame_mat);
    
    if (frame_mat.rows != frame_height_ || frame_mat.cols != frame_width_) {
      LOG(ERROR) << "Frame image has strange dimensions - stream has (" 
              << frame_height_ << ", " << frame_width_
              << "), image has ("
              << frame_mat.rows << ", " << frame_mat.cols << "\n";
      return;
    }
    
    VideoFrame* reflectance_frame = new VideoFrame(frame_width_, 
            frame_height_, 
            3, 
            frame_width_step_,
            frame->pts());
    
    // For the Retinex PDE OpenCV wrapper, the matrix must be fully initialized.
    cv::Mat reflectance;
    reflectance_frame->MatView(&reflectance);        
    
    VideoFrame* shading_frame = new VideoFrame(frame_width_, 
            frame_height_, 
            3, 
            frame_width_step_,
            frame->pts());
    
    cv::Mat shading;
    shading_frame->MatView(&shading);
    
    if (reflectance.rows != frame_mat.rows || reflectance.cols != frame_mat.cols
            || reflectance.channels() != frame_mat.channels()) {
      LOG(ERROR) << "Reflectance image has wrong dimensions!";
      return;
    }
    
    if (shading.rows != frame_mat.rows || shading.cols != frame_mat.cols
            || shading.channels() != frame_mat.channels()) {
      LOG(ERROR) << "Shading image has wrong dimensions!";
      return;
    }
    
    if (reflectance.type() != CV_8UC3) {
      LOG(ERROR) << "Reflectance has wrong type - supposed to be CV_8UC3!";
      return;
    }
    
    if (shading.type() != CV_8UC3) {
      LOG(ERROR) << "Shading has wrong type - supposed to be CV_8UC3!";
      return;
    }
    
    BASE_CLOCK_RESET(0);
    BASE_CLOCK_TOGGLE(0);
    RetinexPDE(frame_mat, options_.threshold, &reflectance);
    RetinexPDE_Shading(frame_mat, reflectance, &shading);
    BASE_CLOCK_TOGGLE(0);
    
    if (input_frames_ % 10 == 0) {
      LOG(INFO) << "Retinex PDE for frame #" << input_frames_ << " (" << BASE_CLOCK_S(0) << "s).\n";
    }
    
    //cv::imwrite(std::to_string(input_frames_) + ".png", reflectance);
    ++input_frames_;
    
    input->push_back(std::shared_ptr<DataFrame>(reflectance_frame));
    input->push_back(std::shared_ptr<DataFrame>(shading_frame));
    output->push_back(input);
  }
  
  bool RetinexPDEUnit::PostProcess(std::list<FrameSetPtr>* append) {
    return false;
  }
}

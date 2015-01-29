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

#include "video_image_writer_unit.h"
#include <opencv2/opencv.hpp>

namespace video_framework {
  
  namespace {
    inline char DirectorySeparator() {
      #ifdef _WIN32
          return '\\';
      #else
          return '/';
      #endif
    }
  }
  
  VideoImageWriterUnit::VideoImageWriterUnit(VideoImageWriterOptions options) : options_(options) {
    
  }
  
  bool VideoImageWriterUnit::OpenStreams(StreamSet* set) {
    LOG(INFO) << "VideoImageWriterUnit reading from stream " << options_.video_stream_name << "\n";
    video_stream_idx_ = FindStreamIdx(options_.video_stream_name, set);
    
    if (video_stream_idx_ < 0) {
      LOG(ERROR) << "Could not find Video stream!\n";
      return false;
    }
    
    return true;
  }
  
  void VideoImageWriterUnit::ProcessFrame(FrameSetPtr input, std::list<FrameSetPtr>* output) {
    const VideoFrame* frame = input->at(video_stream_idx_)->AsPtr<VideoFrame>();
    
    cv::Mat frame_mat;
    frame->MatView(&frame_mat);
    
    std::string path = options_.folder + DirectorySeparator() + std::to_string(input_frames_) + ".png";
    LOG(INFO) << "Writing frame #" << input_frames_ << ": " << path << "\n";
    
    cv::imwrite(options_.folder + DirectorySeparator() + std::to_string(input_frames_) + ".png", frame_mat);
    ++input_frames_;
   
    output->push_back(input);
  }
  
  bool VideoImageWriterUnit::PostProcess(std::list<FrameSetPtr>* append) {
    return false;
  }
} /* video_framework */
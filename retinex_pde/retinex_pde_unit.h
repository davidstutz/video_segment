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

#ifndef VIDEO_SEGMENT_INTRINSIC_RETINEX_PDE_UNIT_H
#define	VIDEO_SEGMENT_INTRINSIC_RETINEX_PDE_UNIT_H

#include "video_framework/video_unit.h"
#include <opencv2/opencv.hpp>
#include <string>

namespace retinex {
  
  using namespace video_framework;
  
  /**
   * Options for the RetinexPDE.
   * 
   * @author David Stutz
   */
  struct RetinexPDEOptions {
    /**
     * The threshold to use for Retinex PDE.
     * For images in RGB 8 bit color space, the threshold should be in (0,255].
     */
    float threshold = 10;
    std::string video_stream_name = "VideoStream";
    std::string reflectance_stream_name = "ReflectanceStream";
    std::string shading_stream_name = "ShadingStream";
  };
  
  /**
   * Retinex PDE Unit to apply the Retinex PDE algorithm on each individual
   * frame.
   * 
   * @author David Stutz
   */
  class RetinexPDEUnit : public VideoUnit {
  public:
    /**
     * Constructor, given Retinex PDE options, constructs
     * a video unit applzing Retinex PDE to each frame.
     * 
     * @param options
     */
    RetinexPDEUnit(RetinexPDEOptions& options);
    
    virtual bool OpenStreams(StreamSet* set);
    virtual void ProcessFrame(FrameSetPtr input, std::list<FrameSetPtr>* output);
    virtual bool PostProcess(std::list<FrameSetPtr>* append);
    
  protected:
    RetinexPDEOptions options_;
    int video_stream_idx_;
    int input_frames_ = 0;
    int frame_width_;
    int frame_height_;
    int frame_width_step_;
  };
}

#endif	/* RETINEX_PDE_UNIT_H */


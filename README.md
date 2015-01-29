# The Video Segmentation Project

Main repository for the Video Segmentation Project.
Online implementation with annotation system available at
www.videosegmentation.com

To build you need the following build dependencies:
- [Boost](http://www.boost.org/)
- [FFMPEG](https://www.ffmpeg.org/) - Works with 2.2.3*.
- [Google protobuffer](https://code.google.com/p/protobuf/)
- [Google logging](https://code.google.com/p/google-glog/)
- [Google gflags](https://code.google.com/p/gflags/)
- [OpenCV](http://opencv.org/) - Works with 2.4.7*.
- [Jsoncpp](https://github.com/open-source-parsers/jsoncpp) (only needed by segment_util).

*We haven't tested it on earlier versions of ffmpeg / opencv yet. Feel free to
try and let us know.

We have put together preliminary commands to build and run seg_tree_sample on
a fresh installation of Ubuntu 14.04 LTS. [You may see those here](https://docs.google.com/document/d/1idKVuSn-8Muhx4bIk5peXzaaYmDgK8bDgw4mgMn8gUY/edit?usp=sharing).

## Documentation

There are several resources for diving directly into the code:

- Using Google Protocol Buffers, C++ Tutorial: [https://developers.google.com/protocol-buffers/docs/cpptutorial](https://developers.google.com/protocol-buffers/docs/cpptutorial).
- [How To Use Google Commandline Flags (gflags)](http://google-gflags.googlecode.com/svn/trunk/doc/gflags.html).
- [How To Use Google Logging Library (glog)](https://google-glog.googlecode.com/svn/trunk/doc/glog.html).
- `seg_tree_minimal` contains a minimal example of `seg_tree_sample` in order to understand the video and segmentation framework.
- `video_image_writer` contains a minimal example of how to use the video pipeline. The corresponding `VideoImageWriterUnit` can be found in `video_framework/video_image_writer_unit.h` and gives an example of a simple video unit.
- Some folders may contain additional `README.md`'s with further documentation.

## Building

Configuration is done via cmake, *outside* the main source tree:

Assuming source is checked out under ~/video_segment

    cd ~/video_segment
    mkdir seg_tree_sample_release
    cd seg_tree_sample_release
    cmake ../seg_tree_sample
    make -j4

## Executables

- seg_tree_sample: 

Runs main segmentation algorithm (over segmentation and hierarchical segmentation).
Example (will create INPUT.mp4.pb segmentation result)

    ./seg_tree_sample --input_file=INPUT.mp4 --logging --write-to-file

- segment_converter:

Render segmentations to images (randomized color), region images (id stored as 24 bit int)
or protobuffers (extract one protobuffer per frame in either binary or text format).
Example (will render images at 10% of hierarchy to OUTPUT_DIR/frameXXXXX.png):

    ./segment_converter --input=INPUT.mp4.pb --bitmap_color=0.1 --output_dir=OUTPUT_DIR

- segment_renderer:

Renders segmentation to video and images with support to parse project JSON files.
Example (to render annotated result to images):

    ./segment_renderer --input_file=INPUT.mp4.pb --output_video_file=OUTPUT.mov

Example (to render segmentation result at 10% of hierarchy to video):

    ./segment_renderer --input_file=INPUT.pb --output_video_file=OUTPUT.mov --render_level=0.1

- segment_viewer: Interactive viewer for segmentation pb files

    ./segment_viewer --input=INPUT.pb

- video_example: Example to run video_framework single threaded or as pipeline

## Known Issues / Questions

* `VideoWriterUnit` tries to use the libx264 codec for files with `.mov` or `.mp4` extensions (note the lowercase), however, libx264 is not listed as dependency and installing libx264 through `sudo apt-get install libx264-dev` may not be the required version for the latest FFMPEG.

## Reference

Algorithm with many improvements is loosely based on

    Matthias Grundmann and Vivek Kwatra and Mei Han and Irfan Essa
    "Efficient Hierarchical Graph Based Video Segmentation"
    IEEE CVPR, 2010

http://www.cc.gatech.edu/cpl/projects/videosegmentation/

## Contributors

- Matthias Grundmann
- Vivek Kwatra
- Mei Han
- Daniel Castro
- Chris McClanahan
- Irfan Essa

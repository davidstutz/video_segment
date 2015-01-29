# Video Framework

## `video_unit.h`

Declares the following classes:

- Basic `Frame` class and derived classes, e.g. `DataFrame`, `ValueFrame`, `VideoFrame` - implement different kind of containers.
- Basic `DataStream` class and derived classes,  e.g. `VideoStream`.
- `VideoUnit` class including documentation/comments.

`VideoUnit` is the base class for all video processing units and pipeline units (`VideoPipelineSink` and `VideoPipelineSource`). Each `VideoUnit` may have several child-`VideoUnit`'s (e.g. using `.attachTo(...)`).

Declares the following functions:

- `PixelFormatToNumChannels`

## VideoWriterUnit `video_writer_unit.h`

## VideoPipelineSink, VideoPipelineSource, VideoPipelineInvoker `video_pipeline.h`

## LuminanceUnit, FlipBGRUnit, ColorTwist `conversion_unit.h`

Declares conversion units:

- `LuminanceUnit` changes the color format of the frames specified by the `PIXEL_FORMAT_*` constants (defined in `video_unit.h`).
- `FlipBGRUnit` flips the B and R in BGR/RGB.
- `ColorTwist` applies a twist weight*color + offset clipped to [0, 255].

## VideoImageWriterUnit `video_image_writer_unit`

Simple unit for demonstration/learning purposes: saves all frames of a video read by a `VideoReaderUnit` at a specific location. See `video_image_writer/video_image_writer.cpp` for usage.
# Video Framework

## `video_unit.h`

Declares the following classes:

- Basic `Frame` class and derived classes, e.g. `DataFrame`, `ValueFrame`, `VideoFrame` - implement different kind of containers.
- Basic `DataStream` class and derived classes,  e.g. `VideoStream`.
- `VideoUnit` class including documentation/comments.

`VideoUnit` is the base class for all video processing units and pipeline units (`VideoPipelineSink` and `VideoPipelineSource`). Each `VideoUnit` may have several child-`VideoUnit`'s (e.g. using `.attachTo(...)`).

## VideoWriterUnit `video_writer_unit.h`

## VideoPipelineSink, VideoPipelineSource, VideoPipelineInvoker `video_pipeline.h`

Both `VideoPipelineSInk` and `VideoPipelinSource` inherit from `VideoUnit`.
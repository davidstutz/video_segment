# Segment Util

## `segmentation.prot`

Google Protobuffer definition of segmentation. A segmentation is stored per frame which is segmented into 2D regions. Each region is described by a rasterization, that is a set of scanlines.

# RenderRegions `segmentation_render.h`

Contains the function `RenderRegions` which assigns a color to each region as defined by the provided `ColorGenerated` (the implementation of `HierarchyColorGenerator` is given in `segmentation_render.cpp`).
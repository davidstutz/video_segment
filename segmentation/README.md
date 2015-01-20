# Segmentation

## Segmentation `segmentation.h`

Usage example:

    Segmentation seg(640, 480, 0);

    // For over-segmentation.
    seg.InitializeOverSegmentation(...);
    seg.AddGenericImage(...);
    seg.AddGenericImage(...);
    seg.ConnectTemporally(...);
    seg.AddGenericImage(...);
    seg.ConnectTemporally(...);
    //More frames ...
    seg.RunOverSegmentation();

    // For hierarchial segmentation.
    seg.InitializeBaseHierarchyLevel(...);
    seg.AddOverSegmentation(...);
    seg.AddOverSegmentation(...);
    seg.AddOverSegmentation(...);
    // More frames ...
    seg.RunHierarchicalSegmentation();

    // Get results.
    seg.AssignUniqueRegionIds(...)
    for (int k = 0; k < num_frames_added; ++k) {
      seg.RetrieveSegmentation3D(...);
    }

Alternatively the individual frames can be connected along the flow direction:

    seg.ConnectTemporallyAlongFlow(...)

Note: The classes `DenseSegmentation` and `HierarchicalSegmentation` are wrappers of this basic segmentation class.

## DenseSegGraphInterface `dense_seg_graph_interface.h`

Provides the basic interface for implementing `DenseSegmentationGraph` in `dense_segmentation_graph.h`.

## DenseSegmentation `dense_segmentation.h`

Wrapper class around the `Segmentation` class - most methods are commented well.

## FastSegmentationGraph `segmentation_graph.h`

Implements a Felzenswalb & Huttenlocher like algorithm that merges regions based on distances between different regions (see `FastSegmentationGraph.SegmentGraph`). The distances are implemented using templates.

## `pixel_distance.h`

Implements several different distances and feature descriptors for regions and pixels.

## `segmentation_unit.h`

Declares the following units:

- `DenseSegmentationUnit`
- `RegionSegmentationUnit`
- `SegmentationWriterUnit`
- `SegmentationReaderUnit`
- `SegmentatioRenderUnit`
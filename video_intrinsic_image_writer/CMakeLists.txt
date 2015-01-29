cmake_minimum_required(VERSION 2.6)
project(video_intrinsic_image_writer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include("${CMAKE_MODULE_PATH}/common.cmake")
include("${CMAKE_SOURCE_DIR}/depend.cmake")

add_executable(video_intrinsic_image_writer video_intrinsic_image_writer.cpp)
apply_dependencies(video_intrinsic_image_writer)
cmake_minimum_required(VERSION 2.6)

project(seg_tree_minimal)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(${CMAKE_MODULE_PATH}/common.cmake)
include("${CMAKE_SOURCE_DIR}/depend.cmake")

set(CMAKE_INCLUDE_CURRENT_DIR 1)
option(NO_X_SUPPORT "Disables interactive output and linking of X libraries" OFF)
option(WITH_QT "Enables QT video and segmentation display" OFF)

if (NO_X_SUPPORT)
  add_definitions(-DNO_X_SUPPORT)
endif(NO_X_SUPPORT)

if (WITH_QT)
  add_definitions(-DWITH_QT)
endif(WITH_QT)

add_executable(seg_tree_minimal seg_tree.cpp)

apply_dependencies(seg_tree_minimal)

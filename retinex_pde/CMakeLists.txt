cmake_minimum_required(VERSION 2.6)

project(retinex_pde_unit)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(${CMAKE_MODULE_PATH}/common.cmake)
include("${CMAKE_SOURCE_DIR}/depend.cmake")

find_package(LIBM REQUIRED)
find_package(FFTW REQUIRED)
find_package(OpenCV2 REQUIRED)

include_directories(${PNG_INCLUDE_DIR} ${FFTW_INCLUDE_DIR} ${LIBM_INCLUDE_DIR} ${OpenCV2_INCLUDE_DIR})

add_library(retinex_pde_lib limare2011-intrinsic-images/retinex_pde_lib.c limare2011-intrinsic-images/norm.c limare2011-intrinsic-images/retinex_pde_lib_wrapper.cpp)
target_link_libraries(retinex_pde_lib ${FFTW_LIBRARIES} ${LIBM_LIBRARIES} ${OpenCV2_LIBRARIES})

add_library(retinex_pde retinex_pde_unit.cpp)
target_link_libraries(retinex_pde retinex_pde_lib)
apply_dependencies(retinex_pde)
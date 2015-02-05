find_package(OpenCV2 REQUIRED)
find_package(GFlags REQUIRED)
find_package(GLog REQUIRED)

set(DEPENDENT_INCLUDES ${GFLAGS_INCLUDE_DIRS}
                       ${GLOG_INCLUDE_DIRS}
                       ${OpenCV2_INCLUDE_DIRS})

set(DEPENDENT_LIBRARIES ${GFLAGS_LIBRARIES} 
                        ${GLOG_LIBRARIES}
                        ${OpenCV2_LIBRARIES})

set(DEPENDENT_PACKAGES base
                       video_framework)
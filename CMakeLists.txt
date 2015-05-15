project("Seein' In")
cmake_minimum_required(VERSION 2.6)

# -DCMAKE_BUILD_TYPE=Release

if( NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release )
endif()

add_definitions(-std=c++11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMakeModules/")

find_package(CUDA QUIET)

find_package(Gflags REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Pangolin REQUIRED)

include_directories(${Pangolin_INCLUDE_DIRS})
link_directories(${Pangolin_LIBRARY_DIRS})
link_libraries(${Pangolin_LIBRARIES})


include_directories(/usr/include/eigen3)

#gflags
include_directories(${gflags_INCLUDE_DIR})
link_libraries(gflags)

#glog
link_libraries(glog)

#protobuf
include_directories(${PROTOBUF_INCLUDE_DIR})
link_libraries(${PROTOBUF_LIBRARY})

set(CAFFE_ROOT /home/tws10/Development/caffe)
include_directories(${CAFFE_ROOT}/include
                    ${CAFFE_ROOT}/build/src/)
link_directories(${CAFFE_ROOT}/build/lib)
link_libraries(caffe)

set(sources
    src/gl_helpers.h
    src/gl_helpers.cpp

    src/mnist_io.h
    src/mnist_io.cpp

    src/seein_in_mouse_handler.h
    src/seein_in_mouse_handler.cpp
)

if (CUDA_FOUND)
message(STATUS "Found CUDA ${CUDA_VERSION_STRING}")
message(STATUS "CUDA Toolkit Root dir: ${CUDA_TOOLKIT_ROOT_DIR}")
message(STATUS "CUDA SDK Root dir: ${CUDA_SDK_ROOT_DIR}")
include_directories(${CUDA_TOOLKIT_ROOT_DIR}/samples/common/inc)

cuda_add_executable(
        # executable
        seeinIn
        # main
        src/seeinIn.cpp
        # support sources
        ${sources}
)
else ()
add_definitions(-DCPU_ONLY)
include_directories(${PROJECT_SOURCE_DIR}/tmp_inc)

add_executable(
        # executable
        seeinIn
        # main
        src/seeinIn.cpp
        # support sources
        ${sources}
)
endif()



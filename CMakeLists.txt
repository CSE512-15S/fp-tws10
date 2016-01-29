project("Seein' In")
cmake_minimum_required(VERSION 2.8)

# -DCMAKE_BUILD_TYPE=Release

if( NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release )
endif()

add_definitions(-std=c++11)

# find packages with modules
find_package(CUDA QUIET)
find_package(Protobuf REQUIRED)
find_package(Pangolin REQUIRED)
find_package(Freetype REQUIRED)

# find packages with pkg-config
find_package(PkgConfig)
pkg_check_modules(fontconfig REQUIRED fontconfig)
pkg_check_modules(gflags REQUIRED libgflags)

# pangolin
include_directories(${Pangolin_INCLUDE_DIRS})
link_directories(${Pangolin_LIBRARY_DIRS})
link_libraries(${Pangolin_LIBRARIES})

# fontconfig
include_directories(${fontconfig_INCLUDEDIR})
link_libraries(${fontconfig_LIBRARIES})

# freetype
include_directories(${FREETYPE_INCLUDE_DIRS})
link_libraries(${FREETYPE_LIBRARIES})

# gflags
include_directories(${gflags_INCLUDE_DIR})
link_libraries(gflags)

# glog
link_libraries(glog)

# protobuf
include_directories(${PROTOBUF_INCLUDE_DIR})
link_libraries(${PROTOBUF_LIBRARY})

# caffe
set(CAFFE_ROOT /usr/local/caffe/ CACHE PATH "path to the caffe root directory")
include_directories(${CAFFE_ROOT}/include
                    ${CAFFE_ROOT}/build/src/)
link_directories(${CAFFE_ROOT}/build/lib)
link_libraries(caffe)
include_directories(/usr/include/hdf5/serial)
link_directories(/usr/lib/hdf5/serial)

set(sources
    src/seeinIn.h
    src/seeinIn.cpp

    src/feature_projector.h
    src/feature_projector.cpp
    src/fonts/font_face.h
    src/fonts/font_face.cpp
    src/fonts/font_manager.h
    src/fonts/font_manager.cpp

    src/mouse_handlers/embedding_view_mouse_handler.h
    src/mouse_handlers/embedding_view_mouse_handler.cpp
    src/mouse_handlers/filter_view_mouse_handler.h
    src/mouse_handlers/filter_view_mouse_handler.cpp
    src/mouse_handlers/multi_embedding_view_mouse_handler.h
    src/mouse_handlers/multi_embedding_view_mouse_handler.cpp
    src/mouse_handlers/single_embedding_view_mouse_handler.h
    src/mouse_handlers/single_embedding_view_mouse_handler.cpp
    src/mouse_handlers/tool_view_mouse_handler.h
    src/mouse_handlers/tool_view_mouse_handler.cpp

    src/shaders/scatter_plot.frag
    src/shaders/scatter_plot.vert
    src/shaders/scatter_plot_shader.h
    src/shaders/scatter_plot_shader.cpp

    src/util/caffe_helpers.h
    src/util/caffe_helpers.cpp
    src/util/geometry.h
    src/util/geometry.cpp
    src/util/gl_helpers.h
    src/util/gl_helpers.cpp
    src/util/image_io.h
    src/util/image_io.cpp
    src/util/selection_manager.h
    src/util/selection_manager.cpp
    src/util/string_format.h
    src/util/string_format.cpp

    src/visualizations/embedding_viz.h
    src/visualizations/embedding_viz.cpp
    src/visualizations/embedding_sub_viz.h
    src/visualizations/embedding_sub_viz.cpp
    src/visualizations/filter_response_viz.h
    src/visualizations/filter_response_viz.cpp
    src/visualizations/multi_embedding_viz.h
    src/visualizations/multi_embedding_viz.cpp
    src/visualizations/single_embedding_viz.h
    src/visualizations/single_embedding_viz.cpp
    src/visualizations/toolbox.h
    src/visualizations/toolbox.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/examples)

# supply compile-time directory to code
set(compileDirectory \"${PROJECT_SOURCE_DIR}\")
add_definitions(-DCOMPILE_DIR=${compileDirectory})

# cache example-building options
set(BUILD_MNIST_EXAMPLE ON CACHE BOOL "build the MNIST example")
set(BUILD_SVHN_EXAMPLE  ON CACHE BOOL "build the SVHN example")

if(BUILD_SVHN_EXAMPLE)
  set(MATLAB_ROOT /usr/local/MATLAB/ CACHE PATH "path to the MATLAB directory")
  include_directories(${MATLAB_ROOT}/extern/include)
  link_libraries(${MATLAB_ROOT}/bin/glnxa64/libmat.so)
  link_libraries(${MATLAB_ROOT}/bin/glnxa64/libmx.so)
endif()

# set up library output
set(BUILD_SHARED_LIBS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
if(CUDA_FOUND)
  message(STATUS "Found CUDA ${CUDA_VERSION_STRING}")
  message(STATUS "CUDA Toolkit Root dir: ${CUDA_TOOLKIT_ROOT_DIR}")
  message(STATUS "CUDA SDK Root dir: ${CUDA_SDK_ROOT_DIR}")
  include_directories(${CUDA_TOOLKIT_ROOT_DIR}/samples/common/inc)

  cuda_add_library(seeinIn ${sources})

  if(BUILD_MNIST_EXAMPLE)
    cuda_add_executable(
        # executable
        seeinInMNIST
        # main
        examples/seeinInMNIST.cpp
        # support sources
        examples/mnist_io.h
        examples/mnist_io.cpp
    )
  endif()

  if(BUILD_SVHN_EXAMPLE)
    cuda_add_executable(
      # executable
      seeinInSVHN
      # main
      examples/seeinInSVHN.cpp
      # support sources
      examples/svhn_io.h
      examples/svhn_io.cpp
    )
  endif()

else()
  add_definitions(-DCPU_ONLY)
  include_directories(${PROJECT_SOURCE_DIR}/tmp_inc)

  add_library(seeinIn ${sources})

  if(BUILD_MNIST_EXAMPLE)
    add_executable(
        # executable
        seeinInMNIST
        # main
        examples/seeinInMNIST.cpp
        # support sources
        examples/mnist_io.h
        examples/mnist_io.cpp
    )
  endif()

  if(BUILD_SVHN_EXAMPLE)
    cuda_add_executable(
      # executable
      seeinInSVHN
      # main
      examples/seeinInSVHN.cpp
      # support sources
      examples/svhn_io.h
      examples/svhn_io.cpp
    )
  endif()

endif()

if(BUILD_MNIST_EXAMPLE)
  target_link_libraries(seeinInMNIST seeinIn)
endif()
if(BUILD_SVHN_EXAMPLE)
  target_link_libraries(seeinInSVHN seeinIn)
endif()




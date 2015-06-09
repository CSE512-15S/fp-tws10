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

#fontconfig
include_directories(/usr/include/fontconfig)
link_libraries(fontconfig)

#freetype
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
link_libraries(${FREETYPE_LIBRARIES})

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

set(tSNE_ROOT /home/tws10/Development/extern/tSNE)
include_directories(${tSNE_ROOT})
set(tSNE_sources
    ${tSNE_ROOT}/sptree.cpp
    ${tSNE_ROOT}/tsne.cpp
)

set(sources
    src/feature_projector.h
    src/feature_projector.cpp
    src/mnist_io.h
    src/mnist_io.cpp

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
        ${tSNE_sources}
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
        ${tSNE_sources}
)
endif()



cmake_minimum_required(VERSION 3.10)
project(gstnvmaxine LANGUAGES C VERSION 0.1.0)

set(LIBRARY_SRC_PATH "${PROJECT_SOURCE_DIR}/src")

set(CMAKE_SKIP_RPATH TRUE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB REQUIRED glib-2.0)

set(GSTREAMER_MINIMUM_VERSION 1.14.0)
set(GSTREAMER_ABI_VERSION "1.0")

pkg_check_modules(PKG_GSTREAMER gstreamer-1.0)
if ( PKG_GSTREAMER_FOUND AND NOT ${PKG_GSTREAMER_VERSION} VERSION_LESS ${GSTREAMER_MINIMUM_VERSION} )
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-${GSTREAMER_ABI_VERSION})
    pkg_check_modules(GSTREAMER-VIDEO REQUIRED gstreamer-video-${GSTREAMER_ABI_VERSION})
    add_definitions(-DGST_API_VERSION_1=1)
    exec_program(${PKG_CONFIG_EXECUTABLE}
            ARGS --variable pluginsdir gstreamer-${GSTREAMER_ABI_VERSION}
            OUTPUT_VARIABLE PKG_GSTREAMER_PLUGIN_DIR)
    exec_program(${PKG_CONFIG_EXECUTABLE}
            ARGS --variable libdir gstreamer-${GSTREAMER_ABI_VERSION}
            OUTPUT_VARIABLE PKG_GSTREAMER_LIB_DIR)
    exec_program(${PKG_CONFIG_EXECUTABLE}
            ARGS --variable includedir gstreamer-${GSTREAMER_ABI_VERSION}
            OUTPUT_VARIABLE PKG_GSTREAMER_INCLUDE_DIR)
else()
   # print error
endif()



set(gst_LIBRARIES
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER-VIDEO_LIBRARIES}
        pthread
        ${GLIB_LIBRARIES}
        ${GLIB_GIO_LIBRARIES}
        ${GLIB_GOBJECT_LIBRARIES}
        )

find_package(CUDA REQUIRED)
find_library(NVINFER nvinfer
        PATHS $ENV{LD_LIBRARY_PATH}
        /usr/local/TensorRT-7.2.2.3/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib64
        /usr/lib
        )
find_library(NVINFERPLUGIN NAMES nvinfer_plugin
        PATHS $ENV{LD_LIBRARY_PATH}
        /usr/local/TensorRT-7.2.2.3/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib64
        /usr/lib
        )
find_library(NVPARSERS NAMES nvparsers
        PATHS $ENV{LD_LIBRARY_PATH}
        /usr/local/TensorRT-7.2.2.3/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib64
        /usr/lib
        )
find_library(NVONNXPARSER NAMES nvonnxparser
        PATHS $ENV{LD_LIBRARY_PATH}
        /usr/local/TensorRT-7.2.2.3/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib64
        /usr/lib
        )

if(CUDA_FOUND AND NVINFER AND NVINFERPLUGIN AND NVPARSERS AND NVONNXPARSER)
    message(STATUS "TensorRT available!")
    message(STATUS "CUDA Libs: ${CUDA_LIBRARIES}")
    message(STATUS "CUDA Headers: ${CUDA_INCLUDE_DIRS}")
    message(STATUS "NVINFER: ${NVINFER}")
    message(STATUS "NVINFERPLUGIN: ${NVINFERPLUGIN}")
    message(STATUS "NVPARSERS: ${NVPARSERS}")
    message(STATUS "NVONNXPARSER: ${NVONNXPARSER}")
    list(APPEND TENSORRT_LIBRARIES nvinfer nvinfer_plugin nvparsers nvonnxparser)
    message(STATUS "TENSORRT_LIBRARIES: ${TENSORRT_LIBRARIES}")
    set(TENSORRT_FOUND ON)
else()
    message("TensorRT NOT Available")
    set(TENSORRT_FOUND OFF)
endif()

add_library(NVVideoEffects INTERFACE)

# found in different locations depending on type of package
find_path(VideoFX_INCLUDES
        NAMES nvVideoEffects.h
        PATHS
        /usr/local/VideoFX/include
        /usr/include/x86_64-linux-gnu
        /usr/include
        REQUIRED
        )

#target_include_directories(NVVideoEffects INTERFACE ${VideoFX_INCLUDES})
set(SDK_INCLUDES_PATH ${VideoFX_INCLUDES})

find_library(VideoFX_LIB
        NAMES libVideoFX.so
        PATHS
        /usr/local/VideoFX/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib64
        /usr/lib
        REQUIRED
        NO_DEFAULT_PATH)

#target_link_libraries(NVVideoEffects INTERFACE "${VideoFX_LIB}")

message(STATUS "VideoFX_LIB: ${VideoFX_LIB}")
message(STATUS "SDK_INCLUDES_PATH: ${SDK_INCLUDES_PATH}")


# Add target for NVCVImage
add_library(NVCVImage INTERFACE)

# found in different locations depending on type of package
find_path(NVCVImage_INCLUDES
        NAMES nvCVImage.h
        PATHS
        /usr/local/VideoFX/include
        /usr/include/x86_64-linux-gnu
        /usr/include
        REQUIRED
        )

#target_include_directories(NVCVImage INTERFACE ${NVCVImage_INCLUDES})


find_library(NVCVImage_LIB
        NAMES libNVCVImage.so
        PATHS
        /usr/local/VideoFX/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib64
        /usr/lib
        REQUIRED
        NO_DEFAULT_PATH)

#target_link_libraries(NVCVImage INTERFACE "${NVCVImage_LIB}")

message(STATUS "NVCVImage_LIB: ${NVCVImage_LIB}")
message(STATUS "NVCVImage_INCLUDES_PATH: ${NVCVImage_INCLUDES}")

find_package (JPEG REQUIRED)

include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER-VIDEO_INCLUDE_DIRS}
        ${CUDA_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${VideoFX_INCLUDES}
        ${NVCVImage_INCLUDES}
        ${JPEG_INCLUDE_DIR}
)

link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARY_DIRS}
        ${GSTREAMER-VIDEO_LIBRARY_DIRS}
        ${CUDA_TOOLKIT_ROOT_DIR}/lib64
        /usr/local/VideoFX/lib
        /usr/local/TensorRT-7.2.2.3/lib
)

set(GST_NV_MAXINE_VIDEOFX_SRC
        "${LIBRARY_SRC_PATH}/videofx.c"
        "${LIBRARY_SRC_PATH}/gstnvmaxinevideofx.c"
        "${LIBRARY_SRC_PATH}/utils.c"
  )
set(GST_NV_MAXINE_VIDEOFX_HDR
        "${LIBRARY_SRC_PATH}/gstnvmaxinevideofx.h"
        "${LIBRARY_SRC_PATH}/videofx.h"
        "${LIBRARY_SRC_PATH}/utils.h"
        )

set(GST_NV_MAXINE_VIDEOFX "${PROJECT_NAME}videofx")

set(GST_NV_MAXINE_META_SRC
        "${LIBRARY_SRC_PATH}/gstnvmaxinemeta.c"
        )

set(GST_NV_MAXINE_META_HDR
        "${LIBRARY_SRC_PATH}/gstnvmaxinemeta.h"
        )

set(GST_NV_MAXINE_META "${PROJECT_NAME}meta")

add_library(${GST_NV_MAXINE_META} SHARED ${GST_NV_MAXINE_META_SRC})
set_target_properties(${GST_NV_MAXINE_META} PROPERTIES PUBLIC_HEADER ${GST_NV_MAXINE_META_HDR})

target_include_directories(${GST_NV_MAXINE_META} PRIVATE
        $<BUILD_INTERFACE:${LIBRARY_SRC_PATH}>
        )

target_include_directories(${GST_NV_MAXINE_META} PUBLIC
        $<BUILD_INTERFACE:${LIBRARY_SRC_PATH}>
        )

target_link_libraries(${GST_NV_MAXINE_META}
        ${gst_LIBRARIES}
        ${JPEG_LIBRARIES}
        )

add_library(${GST_NV_MAXINE_VIDEOFX} SHARED ${GST_NV_MAXINE_VIDEOFX_SRC})

target_include_directories(${GST_NV_MAXINE_VIDEOFX} PRIVATE
        $<BUILD_INTERFACE:${LIBRARY_SRC_PATH}>
        )

target_link_libraries(${GST_NV_MAXINE_VIDEOFX}
        VideoFX
        NVCVImage
        ${TENSORRT_LIBRARIES}
        ${gst_LIBRARIES}
        ${CUDA_LIBRARIES}
        ${GST_NV_MAXINE_META}
        )

install(TARGETS ${GST_NV_MAXINE_META}
        LIBRARY DESTINATION ${PKG_GSTREAMER_LIB_DIR}
        PUBLIC_HEADER DESTINATION ${PKG_GSTREAMER_INCLUDE_DIR}/gst)
install(TARGETS ${GST_NV_MAXINE_VIDEOFX} DESTINATION ${PKG_GSTREAMER_PLUGIN_DIR})
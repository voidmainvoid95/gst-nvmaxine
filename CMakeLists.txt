cmake_minimum_required(VERSION 3.10)
project(gstnvmaxine LANGUAGES C CXX VERSION 0.2.2)

set(LIBRARY_SRC_PATH "${PROJECT_SOURCE_DIR}/src")

set(CMAKE_SKIP_RPATH TRUE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(GST_NV_MAXINE_VIDEOFX_SRC
        "src/videofx.cpp"
        "${LIBRARY_SRC_PATH}/gstnvmaxinevideofx.c"
        "src/utils.cpp"
        
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

if (MSVC)
   set(GSTREAMER_DIR "C:/gstreamer/1.0/msvc_x86_64")
   set(NVMAXINE_DIR "C:/Program\ Files/NVIDIA\ Corporation/NVIDIA\ VideoEffects")
   set(GLIB_DIR "C:/gstreamer/1.0/msvc_x86_64")
   set(JPEG_DIR "C:/libjpeg-turbo64")
   set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

else()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
  #  install(TARGETS ${GST_NV_MAXINE_META}
  #          LIBRARY DESTINATION ${PKG_GSTREAMER_LIB_DIR}
  #          PUBLIC_HEADER DESTINATION ${PKG_GSTREAMER_INCLUDE_DIR}/gst)
  #  install(TARGETS ${GST_NV_MAXINE_VIDEOFX} DESTINATION ${PKG_GSTREAMER_PLUGIN_DIR})
endif()

find_package(Gstreamer REQUIRED)
find_package(NvVideoFx REQUIRED)
find_package(GLIB REQUIRED "gobject")
find_package(JPEG REQUIRED)
find_package(CUDA REQUIRED)

include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIR}
        ${CUDA_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${VideoFX_INCLUDES}
        ${NVCVImage_INCLUDES}
        ${JPEG_INCLUDE_DIR}
)

link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARIES}
        ${CUDA_TOOLKIT_ROOT_DIR}/lib64
        ${JPEG_DIR}/lib
        /usr/local/VideoFX/lib
        /usr/local/TensorRT-7.2.2.3/lib
)

add_library(${GST_NV_MAXINE_META} SHARED ${GST_NV_MAXINE_META_SRC})
set_target_properties(${GST_NV_MAXINE_META} PROPERTIES PUBLIC_HEADER ${GST_NV_MAXINE_META_HDR})

target_include_directories(${GST_NV_MAXINE_META} PRIVATE
        $<BUILD_INTERFACE:${LIBRARY_SRC_PATH}>
        )

target_include_directories(${GST_NV_MAXINE_META} PUBLIC
        $<BUILD_INTERFACE:${LIBRARY_SRC_PATH}>
        )

target_link_libraries(${GST_NV_MAXINE_META}
        ${GSTREAMER_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${JPEG_LIBRARY_RELEASE}
        )
        message(STATUS ${NVMAXINE_LIBRARIES})
if (MSVC)
        set(GST_NV_MAXINE_VIDEOFX_SRC ${GST_NV_MAXINE_VIDEOFX_SRC} ${NVMAXINE_WINDOWS_PROXIES})
endif()
add_library(${GST_NV_MAXINE_VIDEOFX} SHARED ${GST_NV_MAXINE_VIDEOFX_SRC})

target_include_directories(${GST_NV_MAXINE_VIDEOFX} PRIVATE
        $<BUILD_INTERFACE:${LIBRARY_SRC_PATH}>
        )

target_link_libraries(${GST_NV_MAXINE_VIDEOFX} PUBLIC
        ${NVMAXINE_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${GSTREAMER_LIBRARIES}
        ${GST_NV_MAXINE_META}
        ${CUDA_LIBRARIES}
        )

     set(NV_VIDEO_EFFECTS_PATH ${NVMAXINE_DIR}/bin)
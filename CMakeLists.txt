cmake_minimum_required(VERSION 3.10)
project(gstnvmaxine LANGUAGES C CXX VERSION 0.2.2)

set(LIBRARY_SRC_PATH "${PROJECT_SOURCE_DIR}/src")

set(CMAKE_SKIP_RPATH TRUE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(GST_NV_MAXINE_VIDEOFX_SRC
        "${LIBRARY_SRC_PATH}/videofx.cpp"
        "${LIBRARY_SRC_PATH}/gstnvmaxinevideofx.c"
        "${LIBRARY_SRC_PATH}/utils.cpp"
        
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
    if (NOT GSTREAMER_DIR)
        set(GSTREAMER_DIR "C:/gstreamer/1.0/msvc_x86_64")
    endif()
    if (NOT NVMAXINE_DIR)
        set(NVMAXINE_DIR "C:/Program\ Files/NVIDIA\ Corporation/NVIDIA\ VideoEffects")
    endif()
    if (NOT GLIB_DIR)
        set(GLIB_DIR "C:/gstreamer/1.0/msvc_x86_64")
    endif()
    if (NOT JPEG_DIR)
        set(JPEG_DIR "C:/libjpeg-turbo64")
    endif()
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
else()
    if (NOT TENSORRT_DIR)
        set(TENSORRT_DIR "/usr/local/TensorRT-8.2.5.1")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

find_package(GSTREAMER REQUIRED "gstvideo;gstbase")
find_package(NVMAXINE REQUIRED)
find_package(GLIB REQUIRED "gobject")
find_package(JPEG REQUIRED)
find_package(CUDA REQUIRED)

include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${NVMAXINE_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
        ${CUDA_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${JPEG_INCLUDE_DIR}
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
        )

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
        ${JPEG_LIBRARIES}
        )

install(TARGETS ${GST_NV_MAXINE_META}
        LIBRARY DESTINATION ${GSTREAMER_LIBRARY_DIR}
        PUBLIC_HEADER DESTINATION ${GSTREAMER_INCLUDE_DIR}/gst)
install(TARGETS ${GST_NV_MAXINE_VIDEOFX} DESTINATION ${GSTREAMER_PLUGIN_DIR})
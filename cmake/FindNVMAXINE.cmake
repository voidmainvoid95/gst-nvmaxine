file(TO_CMAKE_PATH "$ENV{NVMAXINE_DIR}" TRY1_DIR)
file(TO_CMAKE_PATH "${NVMAXINE_DIR}" TRY2_DIR)
file(GLOB NVMAXINE_DIR ${TRY1_DIR} ${TRY2_DIR})

file(TO_CMAKE_PATH "$ENV{TENSORRT_DIR}" TRY1_DIR)
file(TO_CMAKE_PATH "${TENSORRT_DIR}" TRY2_DIR)
file(GLOB TENSORRT_DIR ${TRY1_DIR} ${TRY2_DIR})

if (MSVC)
    SET(CMAKE_find_library_SUFFIXES ".dll" ${CMAKE_find_library_SUFFIXES})
    SET(NVMAXINE_WINDOWS_PROXIES ${NVMAXINE_DIR}/nvvfx/src/nvCVImageProxy.cpp ${NVMAXINE_DIR}/nvvfx/src/nvVideoEffectsProxy.cpp)
endif()

find_path(VIDEOFX_INCLUDE_DIR NAMES nvVideoEffects.h
        PATHS ${NVMAXINE_DIR}/nvvfx/include
            /usr/local/VideoFX/include
            /usr/include/x86_64-linux-gnu
            /usr/include
        ENV INCLUDE
        )
ADD_LIBRARY(NVVideoEffects INTERFACE)

find_path(NVCVIMAGE_INCLUDE_DIR NAMES nvCVImage.h nvCVStatus.h
        PATHS ${NVMAXINE_DIR}/nvvfx/include
            /usr/local/VideoFX/include
            /usr/include/x86_64-linux-gnu
            /usr/include
        ENV INCLUDE
        )

set(NVMAXINE_INCLUDE_DIRS ${VIDEOFX_INCLUDE_DIR} ${NVCVIMAGE_INCLUDE_DIR})

find_library(VIDEOFX_LIBRARY NAMES NVVideoEffects VideoFX libVideoFX
        PATHS ${NVMAXINE_DIR}/bin
            /usr/local/VideoFX/lib
            /usr/lib/x86_64-linux-gnu
            /usr/lib64
            /usr/lib
        ENV LIB
        NO_SYSTEM_ENVIRONMENT_PATH)

find_library(NVCVIMAGE_LIBRARY NAMES NVCVImage libNVCVImage
        PATHS ${NVMAXINE_DIR}/bin
            /usr/local/VideoFX/lib
            /usr/lib/x86_64-linux-gnu
            /usr/lib64
            /usr/lib
        ENV LIB
        NO_SYSTEM_ENVIRONMENT_PATH)

if (MSVC)
    set(NVMAXINE_LIBRARIES NVVideoEffects)
else()
    find_library(NVINFER nvinfer
            PATHS ${NVMAXINE_DIR}/bin
                ${TENSORRT_DIR}/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            )

    find_library(NVINFERPLUGIN NAMES nvinfer_plugin
            PATHS ${NVMAXINE_DIR}/bin
                ${TENSORRT_DIR}/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            )

    find_library(NVPARSERS NAMES nvparsers
            PATHS ${NVMAXINE_DIR}/bin
                ${TENSORRT_DIR}/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            )

    find_library(NVONNXPARSER NAMES nvonnxparser libnvonnxparser
            PATHS ${NVMAXINE_DIR}/bin
                ${TENSORRT_DIR}/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            )
    target_link_libraries(NVVideoEffects INTERFACE "${VIDEOFX_LIBRARY}")
    add_library(NVCVImage INTERFACE)
    target_link_libraries(NVCVImage INTERFACE "${NVCVIMAGE_LIBRARY}")
    set(NVMAXINE_LIBRARIES NVVideoEffects NVCVImage
            ${NVINFER} ${NVINFERPLUGIN}
            ${NVONNXPARSER} ${NVPARSERS})
endif()

list(REMOVE_DUPLICATES NVMAXINE_INCLUDE_DIRS)
list(REMOVE_DUPLICATES NVMAXINE_LIBRARIES)

if (NVCVIMAGE_LIBRARY AND VIDEOFX_LIBRARY AND NVMAXINE_INCLUDE_DIRS)
    message(STATUS "Found: nvida maxine sdk")
    set(NVMAXINE_FOUND TRUE)
else()
    set(NVMAXINE_FOUND FALSE)
endif()

mark_as_advanced(
    NVMAXINE_FOUND
    NVMAXINE_LIBRARIES
    NVMAXINE_INCLUDE_DIRS
    NVMAXINE_WINDOWS_PROXIES
)
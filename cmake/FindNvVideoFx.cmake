FILE(TO_CMAKE_PATH "$ENV{NVMAXINE_DIR}" TRY1_DIR)
FILE(TO_CMAKE_PATH "${NVMAXINE_DIR}" TRY2_DIR)
FILE(GLOB NVMAXINE_DIR ${TRY1_DIR} ${TRY2_DIR})

FILE(TO_CMAKE_PATH "$ENV{TENSORRT_DIR}" TRY1_DIR)
FILE(TO_CMAKE_PATH "${TENSORRT_DIR}" TRY2_DIR)
FILE(GLOB TENSORRT_DIR ${TRY1_DIR} ${TRY2_DIR})

SET(NVMAXINE_FOUND FALSE)

IF (MSVC)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll" ${CMAKE_FIND_LIBRARY_SUFFIXES})
    SET(NVMAXINE_WINDOWS_PROXIES ${NVMAXINE_DIR}/nvvfx/src/nvCVImageProxy.cpp ${NVMAXINE_DIR}/nvvfx/src/nvVideoEffectsProxy.cpp)
    ADD_LIBRARY(NVVideoEffects INTERFACE)
ELSE()
    FIND_LIBRARY(NVINFER nvinfer
            PATHS ${NVMAXINE_DIR}/bin
                ${TENSORRT_DIR}/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            )

    FIND_LIBRARY(NVINFERPLUGIN NAMES nvinfer_plugin
            PATHS ${NVMAXINE_DIR}/bin
                ${TENSORRT_DIR}/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            )

    FIND_LIBRARY(NVPARSERS NAMES nvparsers
            PATHS ${NVMAXINE_DIR}/bin
                ${TENSORRT_DIR}/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            )

    FIND_LIBRARY(NVONNXPARSER NAMES nvonnxparser
            PATHS ${NVMAXINE_DIR}/bin
                ${TENSORRT_DIR}/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            )

    FIND_PATH(VideoFX_INCLUDES NAMES nvVideoEffects.h
        PATHS ${NVMAXINE_DIR}/nvvfx/include
            /usr/local/VideoFX/include
            /usr/include/x86_64-linux-gnu
            /usr/include
        ENV INCLUDE
        )

    ADD_LIBRARY(NVVideoEffects INTERFACE)
    TARGET_LINK_LIBRARIES(NVVideoEffects INTERFACE "${VideoFX_LIB}")

    FIND_LIBRARY(NVCVImage_LIB NAMES NVVideoEffects VideoFX libVideoFX
    PATHS ${NVMAXINE_DIR}/bin
        /usr/local/VideoFX/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib64
        /usr/lib
    ENV LIB
    NO_SYSTEM_ENVIRONMENT_PATH)

    FIND_PATH(NVCVImage_INCLUDES NAMES nvCVImage.h nvCVStatus.h
        PATHS ${NVMAXINE_DIR}/nvvfx/include
            /usr/local/VideoFX/include
            /usr/include/x86_64-linux-gnu
            /usr/include
        ENV INCLUDE
        )

    FIND_LIBRARY(NVCVImage_LIB NAMES NVCVImage libNVCVImage
            PATHS ${NVMAXINE_DIR}/bin
                /usr/local/VideoFX/lib
                /usr/lib/x86_64-linux-gnu
                /usr/lib64
                /usr/lib
            ENV LIB
            NO_SYSTEM_ENVIRONMENT_PATH)

    ADD_LIBRARY(NVCVImage INTERFACE)
    TARGET_LINK_LIBRARIES(NVCVImage INTERFACE "${NVCVImage_LIB}")
ENDIF()


IF(NVCVImage_LIB AND NVCVImage_INCLUDES AND 
    VideoFX_LIB AND VideoFX_INCLUDES AND 
    NVINFER AND NVINFERPLUGIN AND 
    NVONNXPARSER AND NVPARSERS)
  IF (MSVC)
   SET(NVMAXINE_INCLUDE_DIR ${VideoFX_INCLUDES} ${NVCVImage_INCLUDES} ${NVMAXINE_DIR}/nvvfx/include)
  ELSE()
    SET(NVMAXINE_INCLUDE_DIR ${VideoFX_INCLUDES} ${NVCVImage_INCLUDES})
   ENDIF()
  LIST(REMOVE_DUPLICATES NVMAXINE_INCLUDE_DIR)
  IF (MSVC)
    SET(NVMAXINE_LIBRARIES NVVideoEffects)
  ELSE()
    SET(NVMAXINE_LIBRARIES NVVideoEffects NVCVImage 
                        ${NVINFER} ${NVINFERPLUGIN} 
                        ${NVONNXPARSER} ${NVPARSERS})
  ENDIF()
  LIST(REMOVE_DUPLICATES NVMAXINE_LIBRARIES)
  SET(NVMAXINE_FOUND TRUE)
ENDIF()

MARK_AS_ADVANCED(
    NVMAXINE_FOUND
    NVMAXINE_LIBRARIES
    NVCVImage_INCLUDES
    VideoFX_INCLUDES
    NVINFER
    NVINFERPLUGIN
    NVONNXPARSER
    NVPARSERS
    NVMAXINE_WINDOWS_PROXIES
)
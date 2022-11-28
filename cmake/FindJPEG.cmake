FILE(TO_CMAKE_PATH "$ENV{JPEG_DIR}" TRY1_DIR)
FILE(TO_CMAKE_PATH "${JPEG_DIR}" TRY2_DIR)
FILE(GLOB JPEG_DIR ${TRY1_DIR} ${TRY2_DIR})

FIND_PACKAGE(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_JPEG QUIET jpeg)

find_path(JPEG_INCLUDE_DIR jpeglib.h PATHS
                        ${JPEG_DIR}/include
                        ${PC_JPEG_INCLUDEDIR}
                        ${PC_JPEG_INCLUDE_DIRS}
                        ENV INCLUDE
)

set(jpeg_names ${JPEG_NAMES} jpeg jpeg-static libjpeg libjpeg-static)
foreach(name ${jpeg_names})
  list(APPEND jpeg_names_debug "${name}d")
endforeach()
message(STATUS ${JPEG_INCLUDE_DIR})
if(NOT JPEG_LIBRARY)
  find_library(JPEG_LIBRARY_RELEASE NAMES ${jpeg_names} 
                                    PATHS ${JPEG_DIR}/lib
                                          ${PC_JPEG_LIBDIR}
                                          ${PC_JPEG_LIBRARY_DIRS} 
                                    ENV LIB
                                    NO_SYSTEM_ENVIRONMENT_PATH
                                    }
  find_library(JPEG_LIBRARY_DEBUG NAMES ${jpeg_names_debug}    
                                  PATHS ${JPEG_DIR}/lib
                                        ${PC_JPEG_LIBDIR}
                                        ${PC_JPEG_LIBRARY_DIRS})
  )
  mark_as_advanced(JPEG_LIBRARY_RELEASE JPEG_LIBRARY_DEBUG)
endif()
unset(jpeg_names)
unset(jpeg_names_debug)

if(JPEG_INCLUDE_DIR)
  file(GLOB _JPEG_CONFIG_HEADERS_FEDORA "${JPEG_INCLUDE_DIR}/jconfig*.h")
  file(GLOB _JPEG_CONFIG_HEADERS_DEBIAN "${JPEG_INCLUDE_DIR}/*/jconfig.h")
  set(_JPEG_CONFIG_HEADERS
    "${JPEG_INCLUDE_DIR}/jpeglib.h"
    ${_JPEG_CONFIG_HEADERS_FEDORA}
    ${_JPEG_CONFIG_HEADERS_DEBIAN})
  foreach (_JPEG_CONFIG_HEADER IN LISTS _JPEG_CONFIG_HEADERS)
    if (NOT EXISTS "${_JPEG_CONFIG_HEADER}")
      continue ()
    endif ()
    file(STRINGS "${_JPEG_CONFIG_HEADER}"
      jpeg_lib_version REGEX "^#define[\t ]+JPEG_LIB_VERSION[\t ]+.*")

    if (NOT jpeg_lib_version)
      continue ()
    endif ()

    string(REGEX REPLACE "^#define[\t ]+JPEG_LIB_VERSION[\t ]+([0-9]+).*"
      "\\1" JPEG_VERSION "${jpeg_lib_version}")
    break ()
  endforeach ()
  unset(jpeg_lib_version)
  unset(_JPEG_CONFIG_HEADER)
  unset(_JPEG_CONFIG_HEADERS)
  unset(_JPEG_CONFIG_HEADERS_FEDORA)
  unset(_JPEG_CONFIG_HEADERS_DEBIAN)
endif()

if(JPEG_FOUND)
  set(JPEG_LIBRARIES ${JPEG_LIBRARY})
  set(JPEG_INCLUDE_DIRS "${JPEG_INCLUDE_DIR}")

  if(NOT TARGET JPEG::JPEG)
    add_library(JPEG::JPEG UNKNOWN IMPORTED)
    if(JPEG_INCLUDE_DIRS)
      set_target_properties(JPEG::JPEG PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${JPEG_INCLUDE_DIRS}")
    endif()
    if(EXISTS "${JPEG_LIBRARY}")
      set_target_properties(JPEG::JPEG PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${JPEG_LIBRARY}")
    endif()
    if(EXISTS "${JPEG_LIBRARY_RELEASE}")
      set_property(TARGET JPEG::JPEG APPEND PROPERTY
        IMPORTED_CONFIGURATIONS RELEASE)
      set_target_properties(JPEG::JPEG PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
        IMPORTED_LOCATION_RELEASE "${JPEG_LIBRARY_RELEASE}")
    endif()
    if(EXISTS "${JPEG_LIBRARY_DEBUG}")
      set_property(TARGET JPEG::JPEG APPEND PROPERTY
        IMPORTED_CONFIGURATIONS DEBUG)
      set_target_properties(JPEG::JPEG PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
        IMPORTED_LOCATION_DEBUG "${JPEG_LIBRARY_DEBUG}")
    endif()
  endif()
endif()

mark_as_advanced(JPEG_LIBRARY JPEG_INCLUDE_DIR)
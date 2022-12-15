file(TO_CMAKE_PATH "$ENV{GSTREAMER_DIR}" TRY1_DIR)
file(TO_CMAKE_PATH "${GSTREAMER_DIR}" TRY2_DIR)
file(GLOB GSTREAMER_DIR ${TRY1_DIR} ${TRY2_DIR})

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_GSTREAMER QUIET gstreamer-1.0)

find_path(GSTREAMER_INCLUDE_DIR gst/gst.h
                                    PATHS ${GSTREAMER_DIR}/include/
                                        ${GSTREAMER_DIR}/include/gstreamer-1.0
                                        ${PKG_GSTREAMER_INCLUDEDIR}
                                        ${PKG_GSTREAMER_INCLUDE_DIRS}
                                    ENV INCLUDE DOC "Directory containing gst/gst.h include file")

find_path(GSTREAMER_GSTCONFIG_INCLUDE_DIR gst/gstconfig.h
                                          PATHS ${GSTREAMER_DIR}/include/
                                            ${GSTREAMER_DIR}/include/gstreamer-1.0
                                            ${PKG_GSTREAMER_INCLUDEDIR}
                                            ${PKG_GSTREAMER_INCLUDE_DIRS}
                                          ENV INCLUDE DOC "Directory containing gst/gstconfig.h include file")

set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIR} ${GSTREAMER_GSTCONFIG_INCLUDE_DIR})

find_library(GSTREAMER_LIBRARY NAMES gstreamer gstreamer-1.0 libgstreamer-1.0
        PATHS ${GSTREAMER_DIR}/bin
          ${GSTREAMER_DIR}/win32/bin
          ${GSTREAMER_DIR}/lib
          ${GSTREAMER_DIR}/win32/lib
          ${PKG_GSTREAMER_LIBDIR}
          ${PKG_GSTREAMER_LIBRARY_DIRS}
        ENV LIB
        DOC "gstreamer library to link with"
        NO_SYSTEM_ENVIRONMENT_PATH)

get_filename_component(GSTREAMER_LIBRARY_DIR ${GSTREAMER_LIBRARY} PATH)
set(GSTREAMER_PLUGIN_DIR ${GSTREAMER_LIBRARY_DIR}/gstreamer-1.0)
set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARY})

foreach (_component ${GSTREAMER_FIND_COMPONENTS})
  if (${_component} STREQUAL "gstaudio")
    find_library(GSTREAMER_GSTAUDIO_LIBRARY NAMES gstaudio gstaudio-1.0 libgstaudio-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstaudio library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)

    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTAUDIO_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTAUDIO_LIBRARY})
  elseif (${_component} STREQUAL "gstapp")
    find_library(GSTREAMER_GSTAPP_LIBRARY NAMES gstapp gstapp-1.0 libgstapp-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstapp library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTAPP_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTAPP_LIBRARY})
  elseif (${_component} STREQUAL "gstbase")
    find_library(GSTREAMER_GSTBASE_LIBRARY NAMES gstbase gstbase-1.0 libgstbase-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstbase library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTBASE_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTBASE_LIBRARY})
  elseif (${_component} STREQUAL "gstcontroller")
    find_library(GSTREAMER_GSTCONTROLLER_LIBRARY NAMES gstcontroller gstcontroller-1.0 libgstcontroller-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/bin/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstcontroller library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTCONTROLLER_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTCONTROLLER_LIBRARY})
  elseif (${_component} STREQUAL "gstnet")
    find_library(GSTREAMER_GSTNET_LIBRARY NAMES gstnet gstnet-1.0 libgstnet-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstnet library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTNET_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTNET_LIBRARY})
  elseif (${_component} STREQUAL "gstpbutils")
    find_library(GSTREAMER_GSTPBUTILS_LIBRARY NAMES gstpbutils gstpbutils-1.0 libgstpbutils-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstpbutils library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTPBUTILS_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTPBUTILS_LIBRARY})
  elseif (${_component} STREQUAL "gstriff")
    find_library(GSTREAMER_GSTRIFF_LIBRARY NAMES gstriff gstriff-1.0 libgstriff-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstriff library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTRIFF_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTRIFF_LIBRARY})
  elseif (${_component} STREQUAL "gstrtp")
    find_library(GSTREAMER_GSTRTP_LIBRARY NAMES gstrtp gstrtp-1.0 libgstrtp-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstrtp library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTRTP_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTRTP_LIBRARY})
  elseif (${_component} STREQUAL "gstrtsp")
    find_library(GSTREAMER_GSTRTSP_LIBRARY NAMES gstrtsp gstrtsp-1.0 libgstrtsp-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstrtsp library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTRTSP_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTRTSP_LIBRARY})
  elseif (${_component} STREQUAL "gstsdp")
    find_library(GSTREAMER_GSTSDP_LIBRARY NAMES gstsdp gstsdp-1.0 libgstsdp-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstsdp library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTSDP_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTSDP_LIBRARY})
  elseif (${_component} STREQUAL "gsttag")
    find_library(GSTREAMER_GSTTAG_LIBRARY NAMES gsttag gsttag-1.0 libgsttag-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gsttag library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTTAG_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTTAG_LIBRARY})
  elseif (${_component} STREQUAL "gstvideo")
    find_library(GSTREAMER_GSTVIDEO_LIBRARY NAMES gstvideo gstvideo-1.0 libgstvideo-1.0
            PATHS ${GSTREAMER_DIR}/bin
              ${GSTREAMER_DIR}/win32/bin
              ${GSTREAMER_DIR}/lib
              ${GSTREAMER_DIR}/win32/lib
              ${PKG_GSTREAMER_LIBDIR}
              ${PKG_GSTREAMER_LIBRARY_DIRS}
            ENV LIB
            DOC "gstvideo library to link with"
            NO_SYSTEM_ENVIRONMENT_PATH)
    set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GSTREAMER_GSTVIDEO_LIBRARY)
    set(GSTREAMER_LIBRARIES ${GSTREAMER_LIBRARIES} ${GSTREAMER_GSTVIDEO_LIBRARY})
  endif()
endforeach()

list(REMOVE_DUPLICATES GSTREAMER_INCLUDE_DIRS)
list(REMOVE_DUPLICATES GSTREAMER_LIBRARIES)
if(GSTREAMER_LIBRARIES AND GSTREAMER_INCLUDE_DIR AND GSTREAMER_LIBRARY_DIR)
  message(STATUS "Found: gstreamer-1.0")
  set(GSTREAMER_FOUND TRUE)
else()
  set(GSTREAMER_FOUND FALSE)
endif()

mark_as_advanced(
    GSTREAMER_FOUND
    GSTREAMER_LIBRARY_DIR
    GSTREAMER_INCLUDE_DIR
    GSTREAMER_GSTCONFIG_INCLUDE_DIR
    GSTREAMER_INCLUDE_DIRS
    GSTREAMER_LIBRARIES
)
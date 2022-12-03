file(TO_CMAKE_PATH "$ENV{GLIB_DIR}" TRY1_DIR)
file(TO_CMAKE_PATH "${GLIB_DIR}" TRY2_DIR)
file(GLOB GLIB_DIR ${TRY1_DIR} ${TRY2_DIR})

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_GLIB QUIET glib-2.0)

find_path(GLIBCONFIG_INCLUDE_DIR glibconfig.h
                    PATHS 
                        ${GLIB_DIR}/include
                        ${GLIB_LIBRARY_DIR}
                        ${PKG_GLIB_INCLUDEDIR}
                        ${PKG_GLIB_INCLUDE_DIRS}
                    PATH_SUFFIXES glib-2.0/include
                    ENV INCLUDE DOC "Directory containing glibconfig.h include file")

find_path(GLIB_INCLUDE_DIR glib.h
                    PATHS 
                        ${GLIB_DIR}/include
                        ${GLIB_LIBRARY_DIR}
                        ${PKG_GLIB_INCLUDEDIR}
                        ${PKG_GLIB_INCLUDE_DIRS}
                    PATH_SUFFIXES glib-2.0
                    ENV INCLUDE DOC "Directory containing glib.h include file")

set(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIR} ${GLIBCONFIG_INCLUDE_DIR})

find_library(GLIB_LIBRARY NAMES glib glib-2.0 libglib-2.0
        PATHS ${GLIB_DIR}/bin
        ${GLIB_DIR}/lib
        ${PKG_GLIB_LIBDIR}
        ${PKG_GLIB_LIBRARY_DIRS}
        ENV LIB
        DOC "glib library"
        NO_SYSTEM_ENVIRONMENT_PATH)

get_filename_component(GLIB_LIBRARY_DIR ${GLIB_LIBRARY} PATH)
set(GLIB_LIBRARIES ${GLIB_LIBRARY})

if (EXISTS "${GLIBCONFIG_INCLUDE_DIR}/glibconfig.h")
    file(READ "${GLIBCONFIG_INCLUDE_DIR}/glibconfig.h" GLIBCONFIG_H_CONTENTS)
    string(REGEX MATCH "#define GLIB_MAJOR_VERSION ([0-9]+)" _dummy "${GLIBCONFIG_H_CONTENTS}")
    set(GLIB_VERSION_MAJOR "${CMAKE_MATCH_1}")
    string(REGEX MATCH "#define GLIB_MINOR_VERSION ([0-9]+)" _dummy "${GLIBCONFIG_H_CONTENTS}")
    set(GLIB_VERSION_MINOR "${CMAKE_MATCH_1}")
    string(REGEX MATCH "#define GLIB_MICRO_VERSION ([0-9]+)" _dummy "${GLIBCONFIG_H_CONTENTS}")
    set(GLIB_VERSION_MICRO "${CMAKE_MATCH_1}")
    set(GLIB_VERSION "${GLIB_VERSION_MAJOR}.${GLIB_VERSION_MINOR}.${GLIB_VERSION_MICRO}")
endif ()

foreach (_component ${GLIB_FIND_COMPONENTS})
    if (${_component} STREQUAL "gio")
        find_library(GLIB_GIO_LIBRARIES NAMES gio-2.0 PATHS ${GLIB_LIBRARY_DIR})
        set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GIO_LIBRARIES)
        set(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_GIO_LIBRARIES})
    elseif (${_component} STREQUAL "gobject")
        find_library(GLIB_GOBJECT_LIBRARIES NAMES gobject-2.0 PATHS ${GLIB_LIBRARY_DIR})
        set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GOBJECT_LIBRARIES)
        set(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES})
    elseif (${_component} STREQUAL "gmodule")
        find_library(GLIB_GMODULE_LIBRARIES NAMES gmodule-2.0 PATHS ${GLIB_LIBRARY_DIR})
        set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GMODULE_LIBRARIES)
        set(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_GMODULE_LIBRARIES})
    elseif (${_component} STREQUAL "gthread")
        find_library(GLIB_GTHREAD_LIBRARIES NAMES gthread-2.0 PATHS ${GLIB_LIBRARY_DIR})
        set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GTHREAD_LIBRARIES)
        set(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES})
    elseif (${_component} STREQUAL "gio-unix")
        pkg_check_modules(GIO_UNIX QUIET gio-unix-2.0)
    endif ()
endforeach ()

if (GLIB_LIBRARIES AND GLIB_INCLUDE_DIR AND GLIB_LIBRARY_DIR)
    message(STATUS "Found: glib")
    set(GLIB_FOUND TRUE)
else()
    set(GLIB_FOUND FALSE)
endif()

mark_as_advanced(
    GLIB_FOUND
    GLIB_LIBRARY_DIR
    GLIBCONFIG_INCLUDE_DIR
    GLIB_INCLUDE_DIR
    GLIB_INCLUDE_DIRS
    GLIB_LIBRARIES
)
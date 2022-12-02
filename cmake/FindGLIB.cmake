FILE(TO_CMAKE_PATH "$ENV{GLIB_DIR}" TRY1_DIR)
FILE(TO_CMAKE_PATH "${GLIB_DIR}" TRY2_DIR)
FILE(GLOB GLIB_DIR ${TRY1_DIR} ${TRY2_DIR})

FIND_PACKAGE(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_GLIB QUIET glib-2.0)

FIND_LIBRARY(GLIB_LIBRARIES NAMES glib glib-2.0 libglib-2.0
                                    PATHS ${GLIB_DIR}/bin 
                                        ${GLIB_DIR}/lib 
                                        ${PC_GLIB_LIBDIR}
                                        ${PC_GLIB_LIBRARY_DIRS}
                                    ENV LIB
                                    DOC "glib library"
                                    NO_SYSTEM_ENVIRONMENT_PATH)

GET_FILENAME_COMPONENT(GLIB_LIBRARY_DIR ${GLIB_LIBRARIES} PATH)

FIND_PATH(GLIBCONFIG_INCLUDE_DIR glibconfig.h
                    PATHS 
                        ${GLIB_DIR}/include
                        ${GLIB_LIBRARY_DIR}
                        ${PC_GLIB_INCLUDEDIR}
                        ${PC_GLIB_INCLUDE_DIRS}
                    PATH_SUFFIXES glib-2.0/include
                    ENV INCLUDE DOC "Directory containing glibconfig.h include file")

               
FIND_PATH(GLIB_INCLUDE_DIR glib.h
                    PATHS 
                        ${GLIB_DIR}/include
                        ${GLIB_LIBRARY_DIR}
                        ${PC_GLIB_INCLUDEDIR}
                        ${PC_GLIB_INCLUDE_DIRS}
                    PATH_SUFFIXES glib-2.0
                    ENV INCLUDE DOC "Directory containing glib.h include file")
SET(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIR} ${GLIBCONFIG_INCLUDE_DIR})

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
        FIND_LIBRARY(GLIB_GIO_LIBRARIES NAMES gio-2.0 PATHS ${GLIB_LIBRARY_DIR})
        SET(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GIO_LIBRARIES)
        SET(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_GIO_LIBRARIES})
    elseif (${_component} STREQUAL "gobject")
        FIND_LIBRARY(GLIB_GOBJECT_LIBRARIES NAMES gobject-2.0 PATHS ${GLIB_LIBRARY_DIR})
        SET(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GOBJECT_LIBRARIES)
        SET(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES})
    elseif (${_component} STREQUAL "gmodule")
        FIND_LIBRARY(GLIB_GMODULE_LIBRARIES NAMES gmodule-2.0 PATHS ${GLIB_LIBRARY_DIR})
        SET(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GMODULE_LIBRARIES)
        SET(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_GMODULE_LIBRARIES})
    elseif (${_component} STREQUAL "gthread")
        FIND_LIBRARY(GLIB_GTHREAD_LIBRARIES NAMES gthread-2.0 PATHS ${GLIB_LIBRARY_DIR})
        SET(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GTHREAD_LIBRARIES)
        SET(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES})
    elseif (${_component} STREQUAL "gio-unix")
        PKG_CHECK_MODULES(GIO_UNIX QUIET gio-unix-2.0)
    endif ()
endforeach ()

IF (GLIB_LIBRARIES AND GLIB_INCLUDE_DIRS AND GLIB_INCLUDE_DIR)
    SET(GLIB_FOUND TRUE)
ELSE()
    SET(GLIB_FOUND FALSE)
ENDIF()

MARK_AS_ADVANCED(
    GLIB_FOUND
    GLIB_LIBRARY_DIR
    GLIBCONFIG_INCLUDE_DIR
    GLIB_GIO_LIBRARIES
    GLIB_GIO_UNIX_LIBRARIES
    GLIB_GMODULE_LIBRARIES
    GLIB_GOBJECT_LIBRARIES
    GLIB_GTHREAD_LIBRARIES
    GLIB_INCLUDE_DIR
    GLIB_INCLUDE_DIRS
    GLIB_LIBRARIES
)
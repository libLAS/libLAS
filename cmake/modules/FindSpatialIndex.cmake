###############################################################################
# CMake module to search for SpatialIndex library
#
# On success, the macro sets the following variables:
# SPATIALINDEX_FOUND       = if the library found
# SPATIALINDEX_LIBRARY     = full path to the library
# SPATIALINDEX_INCLUDE_DIR = where to find the library headers 
# SPATIALINDEX_VERSION     = version of library which was found, e.g. "1.4.0"
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
###############################################################################
MESSAGE(STATUS "Searching for SpatialIndex ${SpatialIndex_FIND_VERSION}+ library")

IF(SPATIALINDEX_INCLUDE_DIR)
  # Already in cache, be silent
  SET(SPATIALINDEX_FIND_QUIETLY TRUE)
ENDIF()

IF(WIN32)
  SET(OSGEO4W_IMPORT_LIBRARY spatialindex_i)
  IF(DEFINED ENV{OSGEO4W_ROOT})
    SET(OSGEO4W_ROOT_DIR $ENV{OSGEO4W_ROOT})
    MESSAGE(STATUS "Trying OSGeo4W using environment variable OSGEO4W_ROOT=$ENV{OSGEO4W_ROOT}")
  ELSE()
    SET(OSGEO4W_ROOT_DIR c:/OSGeo4W)
    MESSAGE(STATUS "Trying OSGeo4W using default location OSGEO4W_ROOT=${OSGEO4W_ROOT_DIR}")
  ENDIF()
ENDIF()

FIND_PATH(SPATIALINDEX_INCLUDE_DIR
  NAMES SpatialIndex.h RTree.h
  HINTS
  ${OSGEO4W_ROOT_DIR}/include
  PATHS
  ${OSGEO4W_ROOT_DIR}/include
  PATH_SUFFIXES spatialindex
  DOC "Path to include directory of SpatialIndex library")

SET(SPATIALINDEX_NAMES ${OSGEO4W_IMPORT_LIBRARY} spatialindex)
FIND_LIBRARY(SPATIALINDEX_LIBRARY
  NAMES ${SPATIALINDEX_NAMES}
  PATHS ${OSGEO4W_ROOT_DIR}/lib)

IF (SPATIALINDEX_INCLUDE_DIR)
  SET(SPATIALINDEX_VERSION 0)

  SET(SPATIALINDEX_VERSION_H "${SPATIALINDEX_INCLUDE_DIR}/Version.h")
  FILE(READ ${SPATIALINDEX_VERSION_H} SPATIALINDEX_VERSION_H_CONTENTS)

  IF (DEFINED SPATIALINDEX_VERSION_H_CONTENTS)
    STRING(REGEX REPLACE ".*#define[ \t]SIDX_VERSION_MAJOR[ \t]+([0-9]+).*" "\\1" SIDX_VERSION_MAJOR "${SPATIALINDEX_VERSION_H_CONTENTS}")
    STRING(REGEX REPLACE ".*#define[ \t]SIDX_VERSION_MINOR[ \t]+([0-9]+).*" "\\1" SIDX_VERSION_MINOR "${SPATIALINDEX_VERSION_H_CONTENTS}")
    STRING(REGEX REPLACE ".*#define[ \t]SIDX_VERSION_REV[ \t]+([0-9]+).*"   "\\1" SIDX_VERSION_REV   "${SPATIALINDEX_VERSION_H_CONTENTS}")

    IF(NOT ${SIDX_VERSION_MAJOR} MATCHES "[0-9]+")
      MESSAGE(FATAL_ERROR "SpatialIndex version parsing failed for SIDX_VERSION_MAJOR!")
    ENDIF()
    IF(NOT ${SIDX_VERSION_MINOR} MATCHES "[0-9]+")
      MESSAGE(FATAL_ERROR "SpatialIndex version parsing failed for SIDX_VERSION_MINOR!")
    ENDIF()
    IF(NOT ${SIDX_VERSION_REV} MATCHES "[0-9]+")
      MESSAGE(FATAL_ERROR "SpatialIndex version parsing failed for SIDX_VERSION_REV!")
    ENDIF()

    SET(SPATIALINDEX_VERSION "${SIDX_VERSION_MAJOR}.${SIDX_VERSION_MINOR}.${SIDX_VERSION_REV}"
      CACHE INTERNAL "The version string for SpatialIndex library")

    IF (SPATIALINDEX_VERSION VERSION_EQUAL SpatialIndex_FIND_VERSION OR
        SPATIALINDEX_VERSION VERSION_GREATER SpatialIndex_FIND_VERSION)
      MESSAGE(STATUS "Found SpatialIndex version: ${SPATIALINDEX_VERSION}")
    ELSE()
      MESSAGE(FATAL_ERROR "SpatialIndex version check failed. Version ${SPATIALINDEX_VERSION} was found, at least version ${SpatialIndex_FIND_VERSION} is required")
    ENDIF()
  ELSE()
    MESSAGE(FATAL_ERROR "Failed to open ${SPATIALINDEX_VERSION_H} file")
  ENDIF()

ENDIF()

# Handle the QUIETLY and REQUIRED arguments and set SPATIALINDEX_FOUND to TRUE
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SpatialIndex DEFAULT_MSG SPATIALINDEX_LIBRARY SPATIALINDEX_INCLUDE_DIR)

# TODO: Do we want to mark these as advanced? --mloskot
# http://www.cmake.org/cmake/help/cmake2.6docs.html#command:mark_as_advanced
#MARK_AS_ADVANCED(SPATIALINDEX_LIBRARY SPATIALINDEX_INCLUDE_DIR)

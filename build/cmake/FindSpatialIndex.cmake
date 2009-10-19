###############################################################################
# CMake module to search for SpatialIndex library
#
# On success, the macro sets the following variables:
# SPATIALINDEX_FOUND       = if the library found
# SPATIALINDEX_LIBRARY     = full path to the library
# SPATIALINDEX_INCLUDE_DIR = where to find the library headers 
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
###############################################################################
MESSAGE(STATUS "Searching for SpatialIndex ${SpatialIndex_FIND_VERSION}+ library")
MESSAGE(STATUS "   NOTE: Required version is not checked - to be implemented")

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
    NAMES RTree.h 
    PATH_PREFIXES spatialindex
    PATHS
    ${OSGEO4W_ROOT_DIR}/include)

SET(SPATIALINDEX_NAMES ${OSGEO4W_IMPORT_LIBRARY} spatialindex)
FIND_LIBRARY(SPATIALINDEX_LIBRARY
    NAMES ${SPATIALINDEX_NAMES}
    PATHS
    ${OSGEO4W_ROOT_DIR}/lib)

# Handle the QUIETLY and REQUIRED arguments and set SPATIALINDEX_FOUND to TRUE
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPATIALINDEX DEFAULT_MSG SPATIALINDEX_LIBRARY SPATIALINDEX_INCLUDE_DIR)

# TODO: Do we want to mark these as advanced? --mloskot
# http://www.cmake.org/cmake/help/cmake2.6docs.html#command:mark_as_advanced
#MARK_AS_ADVANCED(SPATIALINDEX_LIBRARY SPATIALINDEX_INCLUDE_DIR)

###############################################################################
#
# CMake module to search for GeoTIFF library
#
# On success, the macro sets the following variables:
# GEOTIFF_FOUND       = if the library found
# GEOTIFF_LIBRARIES   = full path to the library
# GEOTIFF_INCLUDE_DIR = where to find the library headers 
# also defined, but not for general use are
# GEOTIFF_LIBRARY, where to find the PROJ.4 library.
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
# Module source: http://github.com/mloskot/workshop/tree/master/cmake/
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
###############################################################################
MESSAGE(STATUS "Searching for GeoTIFF ${GeoTIFF_FIND_VERSION}+ library")
MESSAGE(STATUS "   NOTE: Required version is not checked - to be implemented")

IF(GEOTIFF_INCLUDE_DIR)
    # Already in cache, be silent
    SET(GEOTIFF_FIND_QUIETLY TRUE)
ENDIF()

IF(WIN32)
    SET(OSGEO4W_IMPORT_LIBRARY geotiff_i)
    IF(DEFINED ENV{OSGEO4W_ROOT})
        SET(OSGEO4W_ROOT_DIR $ENV{OSGEO4W_ROOT})
        MESSAGE(STATUS "Trying OSGeo4W using environment variable OSGEO4W_ROOT=$ENV{OSGEO4W_ROOT}")
    ELSE()
        SET(OSGEO4W_ROOT_DIR c:/OSGeo4W)
        MESSAGE(STATUS "Trying OSGeo4W using default location OSGEO4W_ROOT=${OSGEO4W_ROOT_DIR}")
    ENDIF()
ENDIF()
     
FIND_PATH(GEOTIFF_INCLUDE_DIR
    geotiff.h
    PATH_PREFIXES geotiff
    PATHS
    ${OSGEO4W_ROOT_DIR}/include)

SET(GEOTIFF_NAMES ${OSGEO4W_IMPORT_LIBRARY} geotiff)

FIND_LIBRARY(GEOTIFF_LIBRARY
    NAMES ${GEOTIFF_NAMES}
    PATHS
    ${OSGEO4W_ROOT_DIR}/lib)

IF(GEOTIFF_FOUND)
  SET(GEOTIFF_LIBRARIES ${GEOTIFF_LIBRARY})
ENDIF()

# Handle the QUIETLY and REQUIRED arguments and set GEOTIFF_FOUND to TRUE
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GEOTIFF DEFAULT_MSG GEOTIFF_LIBRARY GEOTIFF_INCLUDE_DIR)

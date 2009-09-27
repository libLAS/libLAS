###############################################################################
# CMake module to search for GeoTIFF library
#
# On success, the macro sets the following variables:
# GEOTIFF_FOUND       = if the library found
# GEOTIFF_LIBRARY     = full path to the library
# GEOTIFF_INCLUDE_DIR = where to find the library headers 
#
# On Unix, macro sets also:
# GEOTIFF_VERSION_STRING = human-readable string containing version of the library
#
# Notes:
# FIND_PATH and FIND_LIBRARY normally search standard locations
# before the specified paths. To search non-standard paths first,
# FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
# and then again with no specified paths to search the default
# locations. When an earlier FIND_* succeeds, subsequent FIND_*s
# searching for the same item do nothing. 
#
# Author: Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################
MESSAGE(STATUS "Searching for GeoTIFF ${GeoTIFF_FIND_VERSION}+ library")

IF(WIN32)

    IF(MINGW)
        FIND_PATH(GEOTIFF_INCLUDE_DIR geotiff.h /usr/local/include /usr/include c:/msys/local/include)
        FIND_LIBRARY(GEOTIFF_LIBRARY NAMES geotiff PATHS /usr/local/lib /usr/lib c:/msys/local/lib)
    ENDIF(MINGW)

    IF(MSVC)
        SET(GEOTIFF_INCLUDE_DIR "$ENV{LIB_DIR}/include" CACHE STRING INTERNAL)
        FIND_LIBRARY(GEOTIFF_LIBRARY NAMES geotiff geotiff_i PATHS "$ENV{LIB_DIR}/lib" /usr/lib c:/msys/local/lib)
    ENDIF(MSVC)
  
ELSEIF(UNIX)

    FIND_PATH(GEOTIFF_INCLUDE_DIR
        geotiff.h
        "$ENV{LIB_DIR}/include"
        "$ENV{LIB_DIR}/include/geotiff"
        /usr/local/include
        /usr/include)
    
    FIND_LIBRARY(GEOTIFF_LIBRARY NAMES geotiff)

ELSE()
    MESSAGE("FindGeoTIFF.cmake: unrecognized or unsupported operating system (use Unix or Windows)")
ENDIF()

IF(GEOTIFF_INCLUDE_DIR AND GEOTIFF_LIBRARY)
    SET(GEOTIFF_FOUND TRUE)
ENDIF()

IF(GEOTIFF_FOUND)
    IF(NOT GEOTIFF_FIND_QUIETLY)
        MESSAGE(STATUS "Found GeoTIFF: ${GEOTIFF_LIBRARY}")
    ENDIF()
ELSE()
    MESSAGE(STATUS "GeoTIFF library not found")
    IF (GEOTIFF_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "GeoTIFF library is required")
    ENDIF()
ENDIF()

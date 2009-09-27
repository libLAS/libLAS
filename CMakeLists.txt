###############################################################################
# Main CMake configuration file for libLAS
#
# Author: Mateusz Loskot <mateusz@loskot.net>
#
# ************************************************************************
# WARNING (mloskot): A PROTOTYPE - WORK IN PROGRESS - FEEL FREE TO IMPROVE
# ************************************************************************
#
###############################################################################
# libLAS general settings
PROJECT(libLAS)

# Name of C++ library
SET(LIBLAS_LIB_NAME las)

# Name of C library
SET(LIBLAS_C_LIB_NAME las_c)

# Choose package components
SET(WITH_UTILITIES TRUE CACHE BOOL "Choose if libLAS utilities should be built")
SET(WITH_TESTS FALSE CACHE BOOL "Choose if libLAS unit tests should be built")

###############################################################################
# CMake settings
CMAKE_MINIMUM_REQUIRED( VERSION 2.4.0 )

SET(CMAKE_COLOR_MAKEFILE ON)

# Path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake ${CMAKE_MODULE_PATH})

###############################################################################
# Build type settings

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel"
        FORCE)
ENDIF()

SET(BUILD_PEDANTIC TRUE CACHE BOOL "Choose compilation in pedantic or relaxed mode")

###############################################################################
# Platform and compiler specific settings

IF(WIN32)
    IF (MSVC)
        IF(BUILD_PEDANTIC)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        ENDIF()

        IF (MSVC80)
            ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
            ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNING)
        ENDIF()
    ENDIF()
ELSE()
    IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wno-long-long -ansi")

        IF(BUILD_PEDANTIC)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
        ENDIF()
        
        IF (CMAKE_COMPILER_IS_GNUCXX)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")
        ENDIF()
    ENDIF()
ENDIF(WIN32)

###############################################################################
# Search for dependencies

# zlib support - optional, default=OFF
SET(WITH_ZLIB FALSE CACHE BOOL "Choose if zlib support should be built")

IF(WITH_ZLIB)
    FIND_PACKAGE(ZLIB)

    IF(ZLIB_FOUND)
        ADD_DEFINITIONS(-DHAVE_ZLIB)
    ENDIF()
ENDIF()

# GDAL/OGR support - optional, default=OFF
SET(WITH_GDAL FALSE CACHE BOOL "Choose if GDAL support should be built")

IF(WITH_GDAL)
    FIND_PACKAGE(GDAL 1.6.0)

    IF (GDAL_FOUND)
	    ADD_DEFINITIONS(-DHAVE_GDAL)
    ENDIF()
ENDIF()

# GeoTIFF support - optional, default=OFF
SET(WITH_GEOTIFF FALSE CACHE BOOL "Choose if GeoTIFF support should be built")

IF(WITH_GEOTIFF)
    FIND_PACKAGE(GeoTIFF 1.2.5)

    IF(GEOTIFF_FOUND)
        ADD_DEFINITIONS(-DHAVE_LIBGEOTIFF)
    ENDIF()
ENDIF()

# Spatial Index support - optional, default=OFF
# TODO

# Oracle support - optional, default=OFF
# TODO

# Boost C++ Libraries support - optional, default=OFF
# TODO

###############################################################################
# List of directories to process

ADD_SUBDIRECTORY(src)

IF(WITH_UTILITIES)
    ADD_SUBDIRECTORY(apps)
ENDIF()

IF(WITH_TESTS)
    ADD_SUBDIRECTORY(test)
ENDIF()

###############################################################################
# Output configuration summary



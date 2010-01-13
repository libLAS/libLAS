###############################################################################
# Main CMake configuration file for libLAS
#
# Author: Mateusz Loskot <mateusz@loskot.net>
#
# ************************************************************************
# WARNING (mloskot): A PROTOTYPE - WORK IN PROGRESS
# Here are details about this work: http://liblas.org/ticket/52
# ************************************************************************
#
###############################################################################
# libLAS general settings
PROJECT(libLAS)

# Name of C++ library
SET(LIBLAS_LIB_NAME las)

# Name of C library
SET(LIBLAS_C_LIB_NAME las_c)

# Name of test suite runner
SET(LIBLAS_UNIT_TEST liblas_test)

# Choose package components
SET(WITH_UTILITIES TRUE CACHE BOOL "Choose if libLAS utilities should be built")
SET(WITH_TESTS FALSE CACHE BOOL "Choose if libLAS unit tests should be built")

# Enable CTest and submissions to libLAS dashboard at CDash
# http://my.cdash.org/index.php?project=libLAS
SET(ENABLE_CTEST FALSE CACHE BOOL
    "Enable CTest to support submissions of results to CDash at http://cdash.org")

###############################################################################
# CMake settings
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

SET(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
MARK_AS_ADVANCED(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake ${CMAKE_MODULE_PATH})

###############################################################################
# General build settings

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel"
        FORCE)
ENDIF()

SET(BUILD_PEDANTIC TRUE CACHE BOOL "Choose compilation in pedantic or relaxed mode")

# TODO: Still testing the output paths --mloskot
SET(LIBLAS_BUILD_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

# Output directory in which to build RUNTIME target files.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LIBLAS_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build LIBRARY target files
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBLAS_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build ARCHIVE target files.
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBLAS_BUILD_OUTPUT_DIRECTORY}) 

###############################################################################
# Platform and compiler specific settings

IF(WIN32)
    IF (MSVC)
        IF(BUILD_PEDANTIC)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        ENDIF()

        IF (MSVC80 OR MSVC90 OR MSVC10)
            ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
            ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNING)
        ENDIF()

        # Generate dot-user file with user-specific settings for Visual Studio project
        SET(MSVC_ENVIRONMENT_PATH "" CACHE STRING
            "Custom PATH for Environment property in Visual Studio project configuration")
        MARK_AS_ADVANCED(MSVC_ENVIRONMENT_PATH)
        SET(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG "${ENVIRONMENT_PATH}")
        MARK_AS_ADVANCED(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG)

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

# Boost C++ Libraries support - optional, default=OFF
SET(WITH_BOOST FALSE CACHE BOOL "Choose if Boost C++ Libraries support should be built")

# TODO: Decide minimum required version of Boost. Is 1.38 OK? --mloskot
IF(WITH_BOOST)
    FIND_PACKAGE(Boost 1.35 REQUIRED iostreams)

    IF(Boost_FOUND AND Boost_iostreams_FOUND)
      INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    ENDIF()
ENDIF()

# zlib support - optional, default=OFF
SET(WITH_ZLIB FALSE CACHE BOOL "Choose if zlib support should be built")

IF(WITH_ZLIB)
    FIND_PACKAGE(ZLIB)

    IF(ZLIB_FOUND)
        INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
        ADD_DEFINITIONS(-DHAVE_ZLIB=1)
    ENDIF()
ENDIF()

# GeoTIFF support - optional, default=OFF
SET(WITH_GEOTIFF FALSE CACHE BOOL "Choose if GeoTIFF support should be built")

IF(WITH_GEOTIFF)
    FIND_PACKAGE(GeoTIFF 1.2.5)

    IF(GEOTIFF_FOUND)
    
        # Confirm required API is available
        INCLUDE(CheckFunctionExists) 
        SET(CMAKE_REQUIRED_LIBRARIES ${GEOTIFF_LIBRARY})
        CHECK_FUNCTION_EXISTS(ST_Create HAVE_ST_CREATE)
    
        IF(NOT HAVE_ST_CREATE)
            SET(GEOTIFF_FOUND) # Reset to NOT found for GeoTIFF
            MESSAGE(FATAL_ERROR "GeoTIFF support requires libgeotiff 1.2.5 or newer.")
        ELSE()
            INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
            ADD_DEFINITIONS(-DHAVE_LIBGEOTIFF=1)
        ENDIF()
    ENDIF()
ENDIF()

# GDAL/OGR support - optional, default=OFF
SET(WITH_GDAL FALSE CACHE BOOL "Choose if GDAL support should be built")

IF(WITH_GDAL)
    IF (NOT GEOTIFF_FOUND)
        MESSAGE(FATAL_ERROR "GDAL support requires GeoTIFF library which was not selected")
    ENDIF()

    FIND_PACKAGE(GDAL 1.6.0)

    IF(GDAL_FOUND)
        # Confirm required API is available
        INCLUDE(CheckFunctionExists) 
        SET(CMAKE_REQUIRED_LIBRARIES ${GDAL_LIBRARY})
        CHECK_FUNCTION_EXISTS(OSRSetEquirectangular2 HAVE_OSRSETEQUIRECTANGULAR2)
    
        IF(NOT HAVE_OSRSETEQUIRECTANGULAR2)
            # Reset to NOT found for GDAL/OGR
            SET(GDAL_FOUND)
            MESSAGE(FATAL_ERROR "GDAL support requires GDAL 1.6.0 or newer")
        ELSE()
            INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})
            ADD_DEFINITIONS(-DHAVE_GDAL=1)
        ENDIF()
    ENDIF()
ENDIF()

# Spatial Index support - optional, default=OFF
SET(WITH_SPATIALINDEX FALSE CACHE BOOL "Choose if SpatialIndex support should be built")

IF(WITH_SPATIALINDEX)
    FIND_PACKAGE(SpatialIndex 1.4.0)

    IF(SPATIALINDEX_FOUND)
        INCLUDE_DIRECTORIES(${SPATIALINDEX_INCLUDE_DIR})
        IF (IS_DIRECTORY ${SPATIALINDEX_INCLUDE_DIR}/spatialindex)
            INCLUDE_DIRECTORIES(${SPATIALINDEX_INCLUDE_DIR}/spatialindex)
        ENDIF()
        ADD_DEFINITIONS(-DHAVE_SPATIALINDEX=1)
    ENDIF()
ENDIF()

# Oracle support - optional, default=OFF
SET(WITH_ORACLE FALSE CACHE BOOL "Choose if Oracle support should be built")

IF(WITH_ORACLE)
    FIND_PACKAGE(Oracle)

    IF(ORACLE_FOUND)
        INCLUDE_DIRECTORIES(${ORACLE_INCLUDE_DIR})
        ADD_DEFINITIONS(-DHAVE_ORACLE=1)
    ENDIF()
ENDIF()

###############################################################################
# Installation settings

IF(WIN32)
    SET(DEFAULT_LIB_SUBDIR lib)
    SET(DEFAULT_DATA_SUBDIR .)
    SET(DEFAULT_INCLUDE_SUBDIR include)

    IF (MSVC)
        SET(DEFAULT_BIN_SUBDIR bin)
    ELSE()
        SET(DEFAULT_BIN_SUBDIR .)
    ENDIF()
ELSE()
    # Common locatoins for Unix and Mac OS X
    SET(DEFAULT_BIN_SUBDIR bin)
    SET(DEFAULT_LIB_SUBDIR lib)
    SET(DEFAULT_DATA_SUBDIR share/liblas)
    SET(DEFAULT_INCLUDE_SUBDIR include)
ENDIF()

# Locations are changeable by user to customize layout of libLAS installation
# (default values are platform-specific)
SET(LIBLAS_BIN_SUBDIR ${DEFAULT_BIN_SUBDIR} CACHE STRING
    "Subdirectory where executables will be installed")
SET(LIBLAS_LIB_SUBDIR ${DEFAULT_LIB_SUBDIR} CACHE STRING
    "Subdirectory where libraries will be installed")
SET(LIBLAS_INCLUDE_SUBDIR ${DEFAULT_INCLUDE_SUBDIR} CACHE STRING
    "Subdirectory where header files will be installed")
SET(LIBLAS_DATA_SUBDIR ${DEFAULT_DATA_SUBDIR} CACHE STRING
    "Subdirectory where data will be installed")

# Mark *_SUBDIR variables as advanced and dedicated to use by power-users only.
MARK_AS_ADVANCED(LIBLAS_BIN_SUBDIR LIBLAS_LIB_SUBDIR LIBLAS_INCLUDE_SUBDIR LIBLAS_DATA_SUBDIR)

# Full paths for the installation
SET(LIBLAS_BIN_DIR ${LIBLAS_BIN_SUBDIR})
SET(LIBLAS_LIB_DIR ${LIBLAS_LIB_SUBDIR})
SET(LIBLAS_INCLUDE_DIR ${LIBLAS_INCLUDE_SUBDIR})
SET(LIBLAS_DATA_DIR ${LIBLAS_DATA_SUBDIR})

###############################################################################
# Installation commands

INSTALL(FILES AUTHORS ChangeLog COPYING INSTALL LICENSE.txt README
    DESTINATION ${LIBLAS_DATA_DIR}/doc)

###############################################################################
# Processing of project directories

ADD_SUBDIRECTORY(src)

IF(WITH_UTILITIES)
    MESSAGE(STATUS "Enable libLAS utilities to build - done")
    ADD_SUBDIRECTORY(apps)
ENDIF()

IF(WITH_TESTS)
    MESSAGE(STATUS "Enable libLAS unit tests to build - done")
    ENABLE_TESTING()


    IF(ENABLE_CTEST)
        MESSAGE(STATUS "Enable CTest to support submissions of results to CDash at http://cdash.org")
        CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
        # Dashboard has been prepared for experiments
        # http://my.cdash.org/index.php?project=libLAS
        INCLUDE(CTest)
        MESSAGE(STATUS "Enable CTest to support submissions of results to CDash at http://cdash.org - done")
    ENDIF()

    ADD_SUBDIRECTORY(test)
ELSE()
    IF(ENABLE_CTEST)
        MESSAGE(WARNING "CTest support requested but WITH_TESTS option not specified to build of libLAS unit tests")
    ENDIF()
ENDIF()

# EOF

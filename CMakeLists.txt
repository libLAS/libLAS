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
project(libLAS)

# Version information
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "6")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

if(WIN32)
    # Name of C++ library
    set(LIBLAS_LIB_NAME liblas)

    # Name of C library
    set(LIBLAS_C_LIB_NAME liblas_c)
else()
    # Name of C++ library
    set(LIBLAS_LIB_NAME las)

    # Name of C library
    set(LIBLAS_C_LIB_NAME las_c)

endif()


# Name of test suite runner
set(LIBLAS_UNIT_TEST liblas_test)

set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# Choose package components
set(WITH_UTILITIES TRUE CACHE BOOL "Choose if libLAS utilities should be built")
set(WITH_TESTS TRUE CACHE BOOL "Choose if libLAS unit tests should be built")

# Enable CTest and submissions to libLAS dashboard at CDash
# http://my.cdash.org/index.php?project=libLAS
set(ENABLE_CTEST FALSE CACHE BOOL
    "Enable CTest to support submissions of results to CDash at http://cdash.org")

###############################################################################
# CMake settings
cmake_minimum_required(VERSION 2.8.0)

set(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${libLAS_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

###############################################################################
# General build settings

# Default to release mode 
set(CMAKE_BUILD_TYPE Release CACHE STRING 
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel"
    FORCE
    )

set(BUILD_PEDANTIC TRUE CACHE BOOL "Choose compilation in pedantic or relaxed mode")

# TODO: Still testing the output paths --mloskot
set(LIBLAS_BUILD_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

# Output directory in which to build RUNTIME target files.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LIBLAS_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build LIBRARY target files
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBLAS_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build ARCHIVE target files.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBLAS_BUILD_OUTPUT_DIRECTORY}) 

file(READ "doc/index.txt" README )
file(WRITE "README.txt" "${README}")

###############################################################################
# Platform and compiler specific settings

if(WIN32)
    if (MSVC)
        if(BUILD_PEDANTIC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        endif()

        if (MSVC80 OR MSVC90 OR MSVC10)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            add_definitions(-D_CRT_NONSTDC_NO_WARNING)
        endif()

        # Generate dot-user file with user-specific settings for Visual Studio project
        set(MSVC_ENVIRONMENT_PATH "" CACHE STRING
            "Custom PATH for Environment property in Visual Studio project configuration")
        mark_as_advanced(MSVC_ENVIRONMENT_PATH)
        set(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG "${ENVIRONMENT_PATH}")
        mark_as_advanced(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG)

    endif()
else()
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wno-long-long -ansi")

        if(BUILD_PEDANTIC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
        endif()
        
        if (CMAKE_COMPILER_IS_GNUCXX)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")
        endif()
    endif()
endif(WIN32)

if (APPLE)
    set(SO_EXT dylib)
    set(CMAKE_FIND_FRAMEWORK "LAST")
elseif(WIN32)
    set(SO_EXT dll)
else()
    set(SO_EXT so)
endif(APPLE)

###############################################################################
# Search for dependencies

# libxml2 support - required, default=ON
set(WITH_LIBXML2 FALSE CACHE BOOL "Choose if libxml2 support should be built ")

if(WITH_LIBXML2)
    find_package(LibXml2)
    mark_as_advanced(CLEAR LIBXML2_FOUND)
    mark_as_advanced(CLEAR LIBXML2_INCLUDE_DIR)
    mark_as_advanced(CLEAR LIBXML2_LIBRARIES) 
    mark_as_advanced(CLEAR LIBXML2_DEFINITIONS)
    if(LIBXML2_FOUND)
        include_directories(${LIBXML2_INCLUDE_DIR})
        add_definitions(-DHAVE_LIBXML2=1)
    endif()
endif()



# Boost C++ Libraries support - optional, default=ON
set(WITH_BOOST TRUE CACHE BOOL "Choose if Boost C++ Libraries support should be built (required)")

# TODO: Decide minimum required version of Boost. Is 1.38 OK? --mloskot
if(WITH_BOOST)
    message(STATUS "Searching for Boost 1.38+ - done")

    find_package(Boost 1.38 REQUIRED iostreams)

    if(Boost_FOUND AND Boost_iostreams_FOUND)
      include_directories(${Boost_INCLUDE_DIRS})
    endif()
endif()



# GeoTIFF support - optional, default=OFF
set(WITH_GEOTIFF FALSE CACHE BOOL "Choose if GeoTIFF support should be built")

if(WITH_GEOTIFF)
    find_package(GeoTIFF 1.2.5)

    if(GEOTIFF_FOUND)
    
        # Confirm required API is available
        include(CheckFunctionExists) 
        set(CMAKE_REQUIRED_LIBRARIES ${GEOTIFF_LIBRARY})
        check_function_exists(ST_Create HAVE_ST_CREATE)
    
        if(NOT HAVE_ST_CREATE)
            set(GEOTIFF_FOUND) # Reset to NOT found for GeoTIFF
            message(FATAL_ERROR "GeoTIFF support requires libgeotiff 1.2.5 or newer.")
        else()
            include_directories(${GEOTIFF_INCLUDE_DIR})
            add_definitions(-DHAVE_LIBGEOTIFF=1)
        endif()
    endif()

    # Determine libTIFF availability, required
    if (GEOTIFF_FOUND)

        find_package(TIFF) 
        mark_as_advanced(CLEAR TIFF_INCLUDE_DIR) 
        mark_as_advanced(CLEAR TIFF_LIBRARIES) 
        if (TIFF_FOUND)
            include_directories(${TIFF_INCLUDE_DIR})
        endif()
    endif()
endif()

# GDAL/OGR support - optional, default=OFF
set(WITH_GDAL FALSE CACHE BOOL "Choose if GDAL support should be built")

if(WITH_GDAL)
    if (NOT GEOTIFF_FOUND)
        message(FATAL_ERROR "GDAL support requires GeoTIFF library which was not selected")
    endif()

    find_package(GDAL 1.6.0)

    if(GDAL_FOUND)
        # Confirm required API is available
        include(CheckFunctionExists) 
        set(CMAKE_REQUIRED_LIBRARIES ${GDAL_LIBRARY})
        check_function_exists(OSRSetEquirectangular2 HAVE_OSRSETEQUIRECTANGULAR2)
    
        if(NOT HAVE_OSRSETEQUIRECTANGULAR2)
            # Reset to NOT found for GDAL/OGR
            set(GDAL_FOUND)
            message(FATAL_ERROR "GDAL support requires GDAL 1.6.0 or newer")
        else()
            include_directories(${GDAL_INCLUDE_DIR})
            add_definitions(-DHAVE_GDAL=1)
        endif()
    endif()
endif()

# Spatial Index support - optional, default=OFF
set(WITH_SPATIALINDEX FALSE CACHE BOOL "Choose if SpatialIndex support should be built")

if(WITH_SPATIALINDEX)
    find_package(SpatialIndex 1.4.0)

    if(SPATIALINDEX_FOUND)
        include_directories(${SPATIALINDEX_INCLUDE_DIR})
        if (IS_DIRECTORY ${SPATIALINDEX_INCLUDE_DIR}/spatialindex)
            include_directories(${SPATIALINDEX_INCLUDE_DIR}/spatialindex)
        endif()
        add_definitions(-DHAVE_SPATIALINDEX=1)
    endif()
endif()

# Oracle support - optional, default=OFF
set(WITH_ORACLE FALSE CACHE BOOL "Choose if Oracle support should be built")

if(WITH_ORACLE)
    find_package(Oracle)

    if(ORACLE_FOUND)
        include_directories(${ORACLE_INCLUDE_DIR})
        add_definitions(-DHAVE_ORACLE=1)
    endif()
endif()

###############################################################################
# Installation settings

if(WIN32)
    set(DEFAULT_LIB_SUBDIR lib)
    set(DEFAULT_DATA_SUBDIR .)
    set(DEFAULT_INCLUDE_SUBDIR include)

    if (MSVC)
        set(DEFAULT_BIN_SUBDIR bin)
    else()
        set(DEFAULT_BIN_SUBDIR .)
    endif()
else()
    # Common locatoins for Unix and Mac OS X
    set(DEFAULT_BIN_SUBDIR bin)
    set(DEFAULT_LIB_SUBDIR lib)
    set(DEFAULT_DATA_SUBDIR share/liblas)
    set(DEFAULT_INCLUDE_SUBDIR include)
endif()

# Locations are changeable by user to customize layout of libLAS installation
# (default values are platform-specific)
set(LIBLAS_BIN_SUBDIR ${DEFAULT_BIN_SUBDIR} CACHE STRING
    "Subdirectory where executables will be installed")
set(LIBLAS_LIB_SUBDIR ${DEFAULT_LIB_SUBDIR} CACHE STRING
    "Subdirectory where libraries will be installed")
set(LIBLAS_INCLUDE_SUBDIR ${DEFAULT_INCLUDE_SUBDIR} CACHE STRING
    "Subdirectory where header files will be installed")
set(LIBLAS_DATA_SUBDIR ${DEFAULT_DATA_SUBDIR} CACHE STRING
    "Subdirectory where data will be installed")

# Mark *_SUBDIR variables as advanced and dedicated to use by power-users only.
mark_as_advanced(LIBLAS_BIN_SUBDIR LIBLAS_LIB_SUBDIR LIBLAS_INCLUDE_SUBDIR LIBLAS_DATA_SUBDIR)

# Full paths for the installation
set(LIBLAS_BIN_DIR ${LIBLAS_BIN_SUBDIR})
set(LIBLAS_LIB_DIR ${LIBLAS_LIB_SUBDIR})
set(LIBLAS_INCLUDE_DIR ${LIBLAS_INCLUDE_SUBDIR})
set(LIBLAS_DATA_DIR ${LIBLAS_DATA_SUBDIR})

###############################################################################
# Installation commands

install(FILES AUTHORS ChangeLog COPYING INSTALL LICENSE.txt README.txt
    DESTINATION ${LIBLAS_DATA_DIR}/doc)

###############################################################################
# Processing of project directories

add_subdirectory(src)

if(WITH_UTILITIES)
    message(STATUS "Enable libLAS utilities to build - done")
    add_subdirectory(apps)
endif()

if(WITH_TESTS)
    message(STATUS "Enable libLAS unit tests to build - done")
    enable_testing()


    if(ENABLE_CTEST)
        message(STATUS "Enable CTest to support submissions of results to CDash at http://cdash.org")
        cmake_minimum_required(VERSION 2.8.0)
        # Dashboard has been prepared for experiments
        # http://my.cdash.org/index.php?project=libLAS
        include(CTest)
        message(STATUS "Enable CTest to support submissions of results to CDash at http://cdash.org - done")
    endif()

    add_subdirectory(test)
else()
    if(ENABLE_CTEST)
        message(WARNING "CTest support requested but WITH_TESTS option not specified to build of libLAS unit tests")
    endif()
endif()

if (WIN32)
    include(BuildOSGeo4W)
endif(WIN32)
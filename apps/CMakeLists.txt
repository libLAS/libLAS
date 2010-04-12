###############################################################################
#
# apps/CMakeLists.txt controls building of libLAS utilities 
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################

include_directories(
    .
    ../include
    ../include/liblas/capi)

###############################################################################
# Collect programs to build

set(LASINFO lasinfo)
set(LASMERGE lasmerge)
set(LAS2LAS las2las)
set(LAS2TXT las2txt)
set(TXT2LAS txt2las)

# Utilities depending on 3rd-pary libraries
if(GDAL_FOUND)
    set(LAS2OGR las2ogr)
endif()

if(SPATIALINDEX_FOUND)
    set(LASINDEX lasindex)
endif()

if(ORACLE_FOUND AND GDAL_FOUND)
    set(LAS2OCI las2oci)
endif()

set(LIBLAS_UTILITIES
    ${LASINFO} ${LASMERGE} ${LAS2LAS} ${LAS2TXT} ${TXT2LAS} 
    ${LAS2OGR} ${LASINDEX} ${LAS2OCI})

# TODO: Experimental and requires testing --mloskot
# Generate user-specific settings for Visual Studio project
set(VCPROJ_USER_REMOTE_MACHINE_DEBUG ${MACHINE_NAME})
set(VCPROJ_USER_ENVIRONMENT_DEBUG "${ENVIRONMENT_PATH}")

if(MSVC)
    foreach(utility ${LIBLAS_UTILITIES})
        set(USER_FILE ${utility}.vcproj.$ENV{USERDOMAIN}.$ENV{USERNAME}.user)
        set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})
        message(STATUS "Generating ${CMAKE_GENERATOR} user-specific settings in ${USER_FILE}")
        configure_file(${CMAKE_SOURCE_DIR}/cmake/libLAS.vcproj.user.template ${OUTPUT_PATH} @ONLY)
    endforeach()
endif()

###############################################################################
# Configure build targets

set(APPS_CPP_DEPENDENCIES
    ${LIBLAS_LIB_NAME}
    ${ZLIB_LIBRARY}
    ${TIFF_LIBRARY}
    ${GEOTIFF_LIBRARY}
    ${GDAL_LIBRARY}
    ${SPATIALINDEX_LIBRARY}
    ${ORACLE_LIBRARY})

# Build lasinfo
if(LASINFO)
    set(LASINFO_SRC lascommon.c ${LASINFO}.c)
    add_executable(${LASINFO} ${LASINFO_SRC})
    target_link_libraries(${LASINFO} ${LIBLAS_C_LIB_NAME})
endif()

# Build las2las
if(LAS2LAS)
    set(LAS2LAS_SRC lascommon.c ${LAS2LAS}.c)
    add_executable(${LAS2LAS} ${LAS2LAS_SRC})
    target_link_libraries(${LAS2LAS} ${LIBLAS_C_LIB_NAME})
endif()

# Build las2txt
if(LAS2TXT)
    set(LAS2TXT_SRC lascommon.c ${LAS2TXT}.c)
    add_executable(${LAS2TXT} ${LAS2TXT_SRC})
    target_link_libraries(${LAS2TXT} ${LIBLAS_C_LIB_NAME})
endif()
 
# Build txt2las
if(TXT2LAS)
    set(TXT2LAS_SRC lascommon.c ${TXT2LAS}.c)
    add_executable(${TXT2LAS} ${TXT2LAS_SRC})
    target_link_libraries(${TXT2LAS} ${LIBLAS_C_LIB_NAME})
endif()

# Build lasmerge
if(LASMERGE)
    set(LASMERGE_SRC lascommon.c ${LASMERGE}.c)
    add_executable(${LASMERGE} ${LASMERGE_SRC})
    target_link_libraries(${LASMERGE} ${LIBLAS_C_LIB_NAME})
endif()

# Build lasindex
if(LASINDEX)
    add_executable(${LASINDEX} lasindex.cpp)
    target_link_libraries(${LASINDEX} ${APPS_CPP_DEPENDENCIES})
endif()

# Build las2ogr
if(LAS2OGR)
    add_executable(${LAS2OGR} las2ogr.cpp)
    target_link_libraries(${LAS2OGR} ${APPS_CPP_DEPENDENCIES})
endif()

# Build las2oci
if(LAS2OCI)
    add_executable(${LAS2OCI} las2oci.cpp oci_wrapper.cpp)
    target_link_libraries(${LAS2OCI} ${APPS_CPP_DEPENDENCIES})
endif()

###############################################################################
# Targets installation

install(TARGETS ${LIBLAS_UTILITIES}
    RUNTIME DESTINATION ${LIBLAS_BIN_DIR}
    LIBRARY DESTINATION ${LIBLAS_LIB_DIR}
    ARCHIVE DESTINATION ${LIBLAS_LIB_DIR})

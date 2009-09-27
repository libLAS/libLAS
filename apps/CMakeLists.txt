###############################################################################
#
# apps/CMakeLists.txt controls building of libLAS utilities 
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################
INCLUDE_DIRECTORIES(
    .
    ../include
    ../include/liblas/capi)

# Standard utilities
SET(LASINFO lasinfo)
SET(LAS2LAS las2las)
SET(LAS2TXT las2txt)
SET(TXT2LAS txt2las)
SET(LASMERGE lasmerge)

# Utilities depending on 3rd-pary libraries
IF(GDAL_FOUND)
    SET(LAS2OGR las2ogr)
ENDIF()

IF(SPATIALINDEX_FOUND)
    SET(LASINDEX lasindex)
ENDIF()

IF(OCI_FOUND)
    SET(LAS2OCI las2oci)
ENDIF()

SET(APPS_CPP_DEPENDENCIES
    ${LIBLAS_LIB_NAME}
    ${GEOTIFF_LIBRARY}
    ${GDAL_LIBRARY}
    ${SPATIALINDEX_LIBRARY}
    ${ZLIB_LIBRARY})

# Build lasinfo
IF(LASINFO)
    SET(LASINFO_SRC lascommon.c ${LASINFO}.c)
    ADD_EXECUTABLE(${LASINFO} ${LASINFO_SRC})
    TARGET_LINK_LIBRARIES(${LASINFO} ${LIBLAS_C_LIB_NAME})
ENDIF(LASINFO)

# Build las2las
IF(LAS2LAS)
    SET(LAS2LAS_SRC lascommon.c ${LAS2LAS}.c)
    ADD_EXECUTABLE(${LAS2LAS} ${LAS2LAS_SRC})
    TARGET_LINK_LIBRARIES(${LAS2LAS} ${LIBLAS_C_LIB_NAME})
ENDIF(LAS2LAS)

# Build las2txt
IF(LAS2TXT)
    SET(LAS2TXT_SRC lascommon.c ${LAS2TXT}.c)
    ADD_EXECUTABLE(${LAS2TXT} ${LAS2TXT_SRC})
    TARGET_LINK_LIBRARIES(${LAS2TXT} ${LIBLAS_C_LIB_NAME})
ENDIF(LAS2TXT)
 
# Build txt2las
IF(TXT2LAS)
    SET(TXT2LAS_SRC lascommon.c ${TXT2LAS}.c)
    ADD_EXECUTABLE(${TXT2LAS} ${TXT2LAS_SRC})
    TARGET_LINK_LIBRARIES(${TXT2LAS} ${LIBLAS_C_LIB_NAME})
ENDIF(TXT2LAS)

# Build lasmerge
IF(LASMERGE)
    SET(LASMERGE_SRC lascommon.c ${LASMERGE}.c)
    ADD_EXECUTABLE(${LASMERGE} ${LASMERGE_SRC})
    TARGET_LINK_LIBRARIES(${LASMERGE} ${LIBLAS_C_LIB_NAME})
ENDIF(LASMERGE)

# Build lasindex
IF(LASINDEX)
    ADD_EXECUTABLE(${LASINDEX} lasindex.cpp)
    TARGET_LINK_LIBRARIES(${LASINDEX} ${APPS_CPP_DEPENDENCIES})
ENDIF(LASINDEX)

# Build las2ogr
IF(LAS2OGR)
    ADD_EXECUTABLE(${LAS2OGR} las2ogr.cpp)
    TARGET_LINK_LIBRARIES(${LAS2OGR} ${APPS_CPP_DEPENDENCIES})
ENDIF(LAS2OGR)

# Build las2oci
IF(LAS2OCI)
    ADD_EXECUTABLE(${LAS2OCI} las2oci.cpp)
    TARGET_LINK_LIBRARIES(${LAS2CI} ${APPS_CPP_DEPENDENCIES})
ENDIF(LAS2OCI)

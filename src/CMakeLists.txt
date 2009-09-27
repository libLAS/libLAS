###############################################################################
#
# src/CMakeLists.txt controls build of libLAS library
#
# Author: Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################

# Collect dependencies configuration
IF(GDAL_FOUND)
    SET(LIBLAS_GDAL_CPP gt_wkt_srs.cpp gt_citation.cpp tifvsi.cpp)
ENDIF()

IF(SPATIALINDEX_FOUND)
    SET(LIBLAS_INDEX_CPP
        index/index.cpp
        index/datastream.cpp
        index/visitor.cpp
        index/query.cpp
        index/storage.cpp)
ENDIF()

SET(LIBLAS_INCLUDE_DIR ../include/liblas)

# 
SET(LIBLAS_HPP
    ${LIBLAS_INCLUDE_DIR}/cstdint.hpp
    ${LIBLAS_INCLUDE_DIR}/exception.hpp
    ${LIBLAS_INCLUDE_DIR}/guid.hpp
    ${LIBLAS_INCLUDE_DIR}/iterator.hpp
    ${LIBLAS_INCLUDE_DIR}/lascolor.hpp
    ${LIBLAS_INCLUDE_DIR}/laserror.hpp
    ${LIBLAS_INCLUDE_DIR}/lasfile.hpp
    ${LIBLAS_INCLUDE_DIR}/lasheader.hpp
    ${LIBLAS_INCLUDE_DIR}/laspoint.hpp
    ${LIBLAS_INCLUDE_DIR}/lasreader.hpp
	${LIBLAS_INCLUDE_DIR}/lasspatialreference.hpp
    ${LIBLAS_INCLUDE_DIR}/lasvariablerecord.hpp
    ${LIBLAS_INCLUDE_DIR}/laswriter.hpp
    ${LIBLAS_INCLUDE_DIR}/liblas.hpp)

SET( LIBLAS_DETAIL_HPP
    ${LIBLAS_INCLUDE_DIR}/detail/endian.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/file.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/fwd.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/reader.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/reader10.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/reader11.hpp
	${LIBLAS_INCLUDE_DIR}/detail/reader12.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/sha1.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/sharedptr.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/timer.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/utility.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/writer.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/writer10.hpp
    ${LIBLAS_INCLUDE_DIR}/detail/writer11.hpp
	${LIBLAS_INCLUDE_DIR}/detail/writer12.hpp
)

SET(LIBLAS_CPP
    lasclassification.cpp
    lascolor.cpp
    laserror.cpp
    lasfile.cpp
    lasheader.cpp
    laspoint.cpp
    lasreader.cpp
    lasspatialreference.cpp
    lasvariablerecord.cpp
    laswriter.cpp)

SET(LIBLAS_DETAIL_CPP
    detail/file.cpp
    detail/reader.cpp
    detail/reader10.cpp
    detail/reader11.cpp
	detail/reader12.cpp
    detail/writer.cpp
    detail/writer10.cpp
    detail/writer11.cpp
	detail/writer12.cpp)

SET(LIBLAS_SOURCES
    ${LIBLAS_HPP}
    ${LIBLAS_DETAIL_HPP}
    ${LIBLAS_CPP}
    ${LIBLAS_DETAIL_CPP}
    ${LIBLAS_GDAL_CPP}
    ${LIBLAS_INDEX_CPP})

SET(LIBLAS_C_SOURCES
    ${LIBLAS_INCLUDE_DIR}/capi/las_config.h
    ${LIBLAS_INCLUDE_DIR}/capi/las_version.h
    ${LIBLAS_INCLUDE_DIR}/capi/liblas.h
    las_c_api.cpp)

SOURCE_GROUP("CMake Files" FILES CMakeLists.txt)
SOURCE_GROUP("Header Files" FILES ${LIBLAS_HPP} ${LIBLAS_DETAIL_HPP})
SOURCE_GROUP("Source Files" FILES ${LIBLAS_CPP} ${LIBLAS_DETAIL_CPP})

# Diable Visual C++ language extensions when building libLAS library
IF (WIN32)
    ADD_DEFINITIONS(/Za)
ENDIF()

INCLUDE_DIRECTORIES(
    ../include 
    ${ZLIB_INCLUDE_DIR}
    ${GDAL_INCLUDE_DIR}
    ${GEOTIFF_INCLUDE_DIR})

# Static library with libLAS C++ core
ADD_LIBRARY(${LIBLAS_LIB_NAME} STATIC ${LIBLAS_SOURCES})

# Shared library with libLAS C API
ADD_LIBRARY(${LIBLAS_C_LIB_NAME} SHARED ${LIBLAS_C_SOURCES})

TARGET_LINK_LIBRARIES(${LIBLAS_C_LIB_NAME}
	${LIBLAS_LIB_NAME}
	${ZLIB_LIBRARIES}
	${GDAL_LIBRARY}
	${GEOTIFF_LIBRARY})

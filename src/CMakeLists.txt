###############################################################################
#
# src/CMakeLists.txt controls building of libLAS library
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################

# Collect dependencies configuration
IF(GDAL_FOUND)
    SET(LIBLAS_GDAL_CPP gt_citation.cpp gt_wkt_srs.cpp tifvsi.cpp)
ENDIF()

IF(SPATIALINDEX_FOUND)
    SET(LIBLAS_INDEX_CPP
        index/datastream.cpp
        index/index.cpp
        index/visitor.cpp
        index/query.cpp
        index/storage.cpp)
ENDIF()

###############################################################################
# Source files specification

SET(LIBLAS_HEADERS_DIR ../include/liblas)


IF(SPATIALINDEX_FOUND)
    SET(LIBLAS_INDEX_HPP
        ${LIBLAS_HEADERS_DIR}/index/datastream.hpp
        ${LIBLAS_HEADERS_DIR}/index/index.hpp
        ${LIBLAS_HEADERS_DIR}/index/visitor.hpp
        ${LIBLAS_HEADERS_DIR}/index/query.hpp
        ${LIBLAS_HEADERS_DIR}/index/storage.hpp)
ENDIF()

SET(LIBLAS_HPP
    ${LIBLAS_HEADERS_DIR}/cstdint.hpp
    ${LIBLAS_HEADERS_DIR}/exception.hpp
    ${LIBLAS_HEADERS_DIR}/guid.hpp
    ${LIBLAS_HEADERS_DIR}/iterator.hpp
    ${LIBLAS_HEADERS_DIR}/lasclassification.hpp
	${LIBLAS_HEADERS_DIR}/lascolor.hpp
    ${LIBLAS_HEADERS_DIR}/laserror.hpp
    ${LIBLAS_HEADERS_DIR}/lasfile.hpp
    ${LIBLAS_HEADERS_DIR}/lasheader.hpp
    ${LIBLAS_HEADERS_DIR}/laspoint.hpp
    ${LIBLAS_HEADERS_DIR}/lasreader.hpp
	${LIBLAS_HEADERS_DIR}/lasspatialreference.hpp
    ${LIBLAS_HEADERS_DIR}/lasvariablerecord.hpp
    ${LIBLAS_HEADERS_DIR}/laswriter.hpp
    ${LIBLAS_HEADERS_DIR}/liblas.hpp)

SET(LIBLAS_DETAIL_HPP
    ${LIBLAS_HEADERS_DIR}/detail/endian.hpp
    ${LIBLAS_HEADERS_DIR}/detail/file.hpp
    ${LIBLAS_HEADERS_DIR}/detail/fwd.hpp
    ${LIBLAS_HEADERS_DIR}/detail/reader.hpp
    ${LIBLAS_HEADERS_DIR}/detail/reader10.hpp
    ${LIBLAS_HEADERS_DIR}/detail/reader11.hpp
	${LIBLAS_HEADERS_DIR}/detail/reader12.hpp
    ${LIBLAS_HEADERS_DIR}/detail/sha1.hpp
    ${LIBLAS_HEADERS_DIR}/detail/sharedptr.hpp
    ${LIBLAS_HEADERS_DIR}/detail/timer.hpp
    ${LIBLAS_HEADERS_DIR}/detail/utility.hpp
    ${LIBLAS_HEADERS_DIR}/detail/writer.hpp
    ${LIBLAS_HEADERS_DIR}/detail/writer10.hpp
    ${LIBLAS_HEADERS_DIR}/detail/writer11.hpp
	${LIBLAS_HEADERS_DIR}/detail/writer12.hpp
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
	${LIBLAS_INDEX_HPP}
    ${LIBLAS_CPP}
    ${LIBLAS_DETAIL_CPP}
    ${LIBLAS_INDEX_CPP}
    ${LIBLAS_GDAL_CPP})

SET(LIBLAS_C_SOURCES
    ${LIBLAS_HEADERS_DIR}/capi/las_config.h
    ${LIBLAS_HEADERS_DIR}/capi/las_version.h
    ${LIBLAS_HEADERS_DIR}/capi/liblas.h
    las_c_api.cpp)

SOURCE_GROUP("CMake Files" FILES CMakeLists.txt)
SOURCE_GROUP("Header Files" FILES ${LIBLAS_HPP} ${LIBLAS_DETAIL_HPP})
SOURCE_GROUP("Source Files" FILES ${LIBLAS_CPP} ${LIBLAS_DETAIL_CPP})

# Diable Visual C++ language extensions when building libLAS library
IF (WIN32)
    IF (MSVC)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Za")
    ENDIF()
ENDIF()

# Standard include directory of libLAS library
INCLUDE_DIRECTORIES(../include)

###############################################################################
# Targets settings

# Static libLAS C++ library
ADD_LIBRARY(${LIBLAS_LIB_NAME} STATIC ${LIBLAS_SOURCES})

# Shared libLAS C library
ADD_LIBRARY(${LIBLAS_C_LIB_NAME} SHARED ${LIBLAS_C_SOURCES})

TARGET_LINK_LIBRARIES(${LIBLAS_C_LIB_NAME}
	${LIBLAS_LIB_NAME}
	${GDAL_LIBRARY}
    ${GEOTIFF_LIBRARY}
	${SPATIALINDEX_LIBRARY}
	${ZLIB_LIBRARIES})

###############################################################################
# Targets installation

INSTALL(TARGETS ${LIBLAS_LIB_NAME} ${LIBLAS_C_LIB_NAME}
    RUNTIME DESTINATION ${LIBLAS_BIN_DIR}
    LIBRARY DESTINATION ${LIBLAS_LIB_DIR}
    ARCHIVE DESTINATION ${LIBLAS_LIB_DIR})

INSTALL(DIRECTORY ${LIBLAS_HEADERS_DIR}
    DESTINATION ${LIBLAS_INCLUDE_DIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

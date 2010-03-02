###############################################################################
#
# src/CMakeLists.txt controls building of libLAS library
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################

# Collect dependencies configuration
if(GDAL_FOUND)
  set(LIBLAS_GDAL_CPP
    gt_citation.cpp
    gt_wkt_srs.cpp
    tifvsi.cpp)
endif()

if(SPATIALINDEX_FOUND)
  set(LIBLAS_INDEX_CPP
    index/datastream.cpp
    index/index.cpp
    index/query.cpp
    index/storage.cpp
    index/visitor.cpp)
endif()

###############################################################################
# Source files specification

set(LIBLAS_HEADERS_DIR ../include/liblas)


if(SPATIALINDEX_FOUND)
  set(LIBLAS_INDEX_HPP
    ${LIBLAS_HEADERS_DIR}/index/datastream.hpp
    ${LIBLAS_HEADERS_DIR}/index/index.hpp
    ${LIBLAS_HEADERS_DIR}/index/query.hpp
    ${LIBLAS_HEADERS_DIR}/index/storage.hpp
    ${LIBLAS_HEADERS_DIR}/index/visitor.hpp)
endif()

set(LIBLAS_HPP
  ${LIBLAS_HEADERS_DIR}/cstdint.hpp
  ${LIBLAS_HEADERS_DIR}/exception.hpp
  ${LIBLAS_HEADERS_DIR}/guid.hpp
  ${LIBLAS_HEADERS_DIR}/iterator.hpp
  ${LIBLAS_HEADERS_DIR}/lasclassification.hpp
  ${LIBLAS_HEADERS_DIR}/lascolor.hpp
  ${LIBLAS_HEADERS_DIR}/laserror.hpp
  ${LIBLAS_HEADERS_DIR}/lasformat.hpp    
  ${LIBLAS_HEADERS_DIR}/lasheader.hpp
  ${LIBLAS_HEADERS_DIR}/laspoint.hpp
  ${LIBLAS_HEADERS_DIR}/lasreader.hpp
  ${LIBLAS_HEADERS_DIR}/lasspatialreference.hpp
  ${LIBLAS_HEADERS_DIR}/lasvariablerecord.hpp
  ${LIBLAS_HEADERS_DIR}/lasversion.hpp
  ${LIBLAS_HEADERS_DIR}/laswriter.hpp
  ${LIBLAS_HEADERS_DIR}/liblas.hpp)

set(LIBLAS_DETAIL_HPP
  ${LIBLAS_HEADERS_DIR}/detail/endian.hpp
  ${LIBLAS_HEADERS_DIR}/detail/fwd.hpp
  ${LIBLAS_HEADERS_DIR}/detail/reader/reader.hpp
  ${LIBLAS_HEADERS_DIR}/detail/reader/header.hpp
  ${LIBLAS_HEADERS_DIR}/detail/reader/point.hpp
  ${LIBLAS_HEADERS_DIR}/detail/sha1.hpp
  ${LIBLAS_HEADERS_DIR}/detail/sharedptr.hpp
  ${LIBLAS_HEADERS_DIR}/detail/timer.hpp
  ${LIBLAS_HEADERS_DIR}/detail/utility.hpp
  ${LIBLAS_HEADERS_DIR}/detail/writer/writer.hpp
  ${LIBLAS_HEADERS_DIR}/detail/writer/base.hpp
  ${LIBLAS_HEADERS_DIR}/detail/writer/point.hpp
  ${LIBLAS_HEADERS_DIR}/detail/writer/header.hpp    
  )

set(LIBLAS_CPP
  lasclassification.cpp
  lascolor.cpp
  laserror.cpp
  lasformat.cpp
  lasheader.cpp
  laspoint.cpp
  lasreader.cpp
  lasspatialreference.cpp
  lasvariablerecord.cpp
  laswriter.cpp)

set(LIBLAS_DETAIL_CPP
  detail/reader/header.cpp
  detail/reader/point.cpp
  detail/reader/reader.cpp
  detail/writer/base.cpp
  detail/writer/header.cpp
  detail/writer/point.cpp
  detail/writer/writer.cpp)

set(LIBLAS_SOURCES
  ${LIBLAS_HPP}
  ${LIBLAS_DETAIL_HPP}
  ${LIBLAS_INDEX_HPP}
  ${LIBLAS_CPP}
  ${LIBLAS_DETAIL_CPP}
  ${LIBLAS_INDEX_CPP}
  ${LIBLAS_GDAL_CPP})

set(LIBLAS_C_SOURCES
  ${LIBLAS_HEADERS_DIR}/capi/las_config.h
  ${LIBLAS_HEADERS_DIR}/capi/las_version.h
  ${LIBLAS_HEADERS_DIR}/capi/liblas.h
  las_c_api.cpp)

source_group("CMake Files" FILES CMakeLists.txt)
source_group("Header Files" FILES ${LIBLAS_HPP} ${LIBLAS_DETAIL_HPP})
source_group("Source Files" FILES ${LIBLAS_CPP} ${LIBLAS_DETAIL_CPP})

# Diable Visual C++ language extensions when building libLAS library
if (WIN32)
  if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Za")
  endif()
endif()

# Standard include directory of libLAS library
include_directories(../include)

###############################################################################
# Targets settings

# Static libLAS C++ library
add_library(${LIBLAS_LIB_NAME} STATIC ${LIBLAS_SOURCES})

# Shared libLAS C library
add_library(${LIBLAS_C_LIB_NAME} SHARED ${LIBLAS_C_SOURCES})

target_link_libraries(${LIBLAS_C_LIB_NAME}
  ${LIBLAS_LIB_NAME}
  ${ZLIB_LIBRARIES}
  ${GEOTIFF_LIBRARY}
  ${GDAL_LIBRARY}
  ${SPATIALINDEX_LIBRARY})

###############################################################################
# Targets installation

install(TARGETS ${LIBLAS_LIB_NAME} ${LIBLAS_C_LIB_NAME}
  RUNTIME DESTINATION ${LIBLAS_BIN_DIR}
  LIBRARY DESTINATION ${LIBLAS_LIB_DIR}
  ARCHIVE DESTINATION ${LIBLAS_LIB_DIR})

install(DIRECTORY ${LIBLAS_HEADERS_DIR}
  DESTINATION ${LIBLAS_INCLUDE_DIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

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

###############################################################################
# Source files specification

set(LIBLAS_HEADERS_DIR ../include/liblas)

set(LIBLAS_HPP
  ${LIBLAS_HEADERS_DIR}/chipper.hpp
  ${LIBLAS_HEADERS_DIR}/exception.hpp
  ${LIBLAS_HEADERS_DIR}/export.hpp
  ${LIBLAS_HEADERS_DIR}/factory.hpp 
  ${LIBLAS_HEADERS_DIR}/iterator.hpp
  ${LIBLAS_HEADERS_DIR}/bounds.hpp
  ${LIBLAS_HEADERS_DIR}/classification.hpp
  ${LIBLAS_HEADERS_DIR}/color.hpp
  ${LIBLAS_HEADERS_DIR}/dimension.hpp  
  ${LIBLAS_HEADERS_DIR}/error.hpp
  ${LIBLAS_HEADERS_DIR}/filter.hpp
  ${LIBLAS_HEADERS_DIR}/header.hpp
  ${LIBLAS_HEADERS_DIR}/index.hpp
  ${LIBLAS_HEADERS_DIR}/point.hpp
  ${LIBLAS_HEADERS_DIR}/reader.hpp
  ${LIBLAS_HEADERS_DIR}/schema.hpp
  ${LIBLAS_HEADERS_DIR}/spatialreference.hpp
  ${LIBLAS_HEADERS_DIR}/transform.hpp  
  ${LIBLAS_HEADERS_DIR}/variablerecord.hpp
  ${LIBLAS_HEADERS_DIR}/writer.hpp
  ${LIBLAS_HEADERS_DIR}/liblas.hpp
  ${LIBLAS_HEADERS_DIR}/utility.hpp
  ${LIBLAS_HEADERS_DIR}/version.hpp)

set(LIBLAS_EXTERNAL_HPP)

set(LIBLAS_EXTERNAL_PROPERTY_TREE_HPP
  ${LIBLAS_HEADERS_DIR}/external/property_tree/exceptions.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/id_translator.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/info_parser.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/ini_parser.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/json_parser.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/ptree.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/ptree_fwd.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/ptree_serialization.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/stream_translator.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/string_path.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/xml_parser.hpp)

set(LIBLAS_EXTERNAL_PROPERTY_TREE_DETAIL_HPP
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/exception_implementation.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/file_parser_error.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/info_parser_error.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/info_parser_read.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/info_parser_utils.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/info_parser_write.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/info_parser_writer_settings.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/json_parser_error.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/json_parser_read.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/json_parser_write.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/ptree_implementation.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/ptree_utils.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/rapidxml.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/xml_parser_error.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/xml_parser_flags.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/xml_parser_read_rapidxml.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/xml_parser_utils.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/xml_parser_write.hpp
  ${LIBLAS_HEADERS_DIR}/external/property_tree/detail/xml_parser_writer_settings.hpp)

set(LIBLAS_DETAIL_HPP
  ${LIBLAS_HEADERS_DIR}/detail/binary.hpp
  ${LIBLAS_HEADERS_DIR}/detail/endian.hpp
  ${LIBLAS_HEADERS_DIR}/detail/fwd.hpp
  ${LIBLAS_HEADERS_DIR}/detail/pointrecord.hpp
  ${LIBLAS_HEADERS_DIR}/detail/timer.hpp
  ${LIBLAS_HEADERS_DIR}/detail/private_utility.hpp
  ${LIBLAS_HEADERS_DIR}/detail/singleton.hpp
  ${LIBLAS_HEADERS_DIR}/detail/zippoint.hpp)

set(LIBLAS_DETAIL_INDEX_HPP
  ${LIBLAS_HEADERS_DIR}/detail/index/indexoutput.hpp
  ${LIBLAS_HEADERS_DIR}/detail/index/indexcell.hpp)
  
set(LIBLAS_DETAIL_READER_HPP
  ${LIBLAS_HEADERS_DIR}/detail/reader/cachedreader.hpp
  ${LIBLAS_HEADERS_DIR}/detail/reader/reader.hpp  
  ${LIBLAS_HEADERS_DIR}/detail/reader/zipreader.hpp  
  ${LIBLAS_HEADERS_DIR}/detail/reader/header.hpp
  )

set(LIBLAS_DETAIL_WRITER_HPP
  ${LIBLAS_HEADERS_DIR}/detail/writer/writer.hpp
  ${LIBLAS_HEADERS_DIR}/detail/writer/zipwriter.hpp
  ${LIBLAS_HEADERS_DIR}/detail/writer/point.hpp
  ${LIBLAS_HEADERS_DIR}/detail/writer/header.hpp)

set(LIBLAS_CPP
  chipper.cpp
  factory.cpp
  classification.cpp
  color.cpp
  dimension.cpp
  error.cpp
  filter.cpp
  header.cpp
  index.cpp
  point.cpp
  reader.cpp
  spatialreference.cpp
  schema.cpp
  transform.cpp
  utility.cpp
  variablerecord.cpp
  writer.cpp
  version.cpp)

set(LIBLAS_DETAIL_CPP
  detail/utility.cpp
  detail/sha1.cpp
  detail/zippoint.cpp
)
  
set(LIBLAS_DETAIL_INDEX_CPP
  detail/index/indexcell.cpp
  detail/index/indexoutput.cpp)

set(LIBLAS_DETAIL_READER_CPP
  detail/reader/header.cpp
  detail/reader/reader.cpp
  detail/reader/zipreader.cpp
  detail/reader/cachedreader.cpp)

set(LIBLAS_DETAIL_WRITER_CPP
  detail/writer/header.cpp
  detail/writer/point.cpp
  detail/writer/zipwriter.cpp
  detail/writer/writer.cpp)

# Group source files for IDE source explorers (e.g. Visual Studio)
source_group("CMake Files" FILES CMakeLists.txt)
source_group("Header Files" FILES ${LIBLAS_HPP})
source_group("Header Files\\external" FILES ${LIBLAS_EXTERNAL_HPP})
source_group("Header Files\\external\\property_tree" FILES ${LIBLAS_EXTERNAL_PROPERTY_TREE_HPP})
source_group("Header Files\\external\\property_tree\\detail" FILES ${LIBLAS_EXTERNAL_PROPERTY_TREE_DETAIL_HPP})
source_group("Header Files\\detail" FILES ${LIBLAS_DETAIL_HPP})
source_group("Header Files\\detail\\index" FILES ${LIBLAS_DETAIL_INDEX_HPP})
source_group("Header Files\\detail\\reader" FILES ${LIBLAS_DETAIL_READER_HPP})
source_group("Header Files\\detail\\writer" FILES ${LIBLAS_DETAIL_WRITER_HPP})
source_group("Source Files" FILES ${LIBLAS_CPP})
source_group("Source Files\\detail" FILES ${LIBLAS_DETAIL_CPP})
source_group("Source Files\\detail\\index" FILES ${LIBLAS_DETAIL_INDEX_CPP})
source_group("Source Files\\detail\\reader" FILES ${LIBLAS_DETAIL_READER_CPP})
source_group("Source Files\\detail\\writer" FILES ${LIBLAS_DETAIL_WRITER_CPP})
if(GDAL_FOUND)
    source_group("Source Files\\gdal" FILES ${LIBLAS_GDAL_CPP})
endif()

# Diable Visual C++ language extensions when building libLAS library
# Need this enabled for boost to work -- hobu
#if (WIN32)
#  if (MSVC)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Za")
#  endif()
#endif()

###############################################################################
# Targets settings

set(LIBLAS_SOURCES
  ${LIBLAS_HPP}
  ${LIBLAS_EXTERNAL_HPP}
  ${LIBLAS_EXTERNAL_PROPERTY_TREE_HPP}
  ${LIBLAS_EXTERNAL_PROPERTY_TREE_DETAIL_HPP}
  ${LIBLAS_DETAIL_HPP}
  ${LIBLAS_DETAIL_READER_HPP}
  ${LIBLAS_DETAIL_WRITER_HPP}
  ${LIBLAS_DETAIL_INDEX_CPP}
  ${LIBLAS_CPP}
  ${LIBLAS_DETAIL_CPP}
  ${LIBLAS_DETAIL_READER_CPP}
  ${LIBLAS_DETAIL_WRITER_CPP}
  ${LIBLAS_GDAL_CPP})

set(LIBLAS_C_SOURCES
  ${LIBLAS_HEADERS_DIR}/capi/las_config.h
  ${LIBLAS_HEADERS_DIR}/capi/las_version.h
  ${LIBLAS_HEADERS_DIR}/capi/liblas.h
  c_api.cpp)

# NOTE:
# This hack is required to correctly link static into shared library.
# Such practice is not recommended as not portable, instead each library,
# static and shared should be built from sources separately.
#if(UNIX)
#  add_definitions("-fPIC")
#endif()

if(WIN32)
    add_definitions("-DLAS_DLL_EXPORT=1")
if (NOT WITH_STATIC_LASZIP)
    add_definitions("-DLASZIP_DLL_IMPORT=1")
endif()
endif()

add_library(las SHARED ${LIBLAS_SOURCES})
add_library(las_c SHARED ${LIBLAS_C_SOURCES})

target_link_libraries(las
  ${TIFF_LIBRARY}
  ${GEOTIFF_LIBRARY}
  ${GDAL_LIBRARY}
  ${LASZIP_LIBRARY}
  ${Boost_LIBRARIES})

target_link_libraries(las_c
  las )

set_target_properties(las
  PROPERTIES
  VERSION "${LIBLAS_LIB_VERSION}"
  SOVERSION "${LIBLAS_LIB_SOVERSION}"
  OUTPUT_NAME "${LIBLAS_LIB_NAME}")
target_include_directories(las
  PUBLIC
    $<BUILD_INTERFACE:${libLAS_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${LIBLAS_INCLUDE_DIR}>)

set_target_properties(las_c
  PROPERTIES
  VERSION "${LIBLAS_C_LIB_VERSION}"
  SOVERSION "${LIBLAS_C_LIB_SOVERSION}"
  OUTPUT_NAME "${LIBLAS_C_LIB_NAME}")
target_include_directories(las_c
  PUBLIC
    $<BUILD_INTERFACE:${libLAS_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${LIBLAS_INCLUDE_DIR}>)

###############################################################################
# Targets installation.  The EXPORT clause specifies a depends target
# which packages up information about the libraries for
# liblas-config.cmake.

install(TARGETS las las_c
  EXPORT depends
  RUNTIME DESTINATION ${LIBLAS_BIN_DIR}
  LIBRARY DESTINATION ${LIBLAS_LIB_DIR}
  ARCHIVE DESTINATION ${LIBLAS_LIB_DIR})

install(DIRECTORY ${LIBLAS_HEADERS_DIR}
  DESTINATION ${LIBLAS_INCLUDE_DIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

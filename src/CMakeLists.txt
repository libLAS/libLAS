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
  ${LIBLAS_HEADERS_DIR}/cstdint.hpp
  ${LIBLAS_HEADERS_DIR}/exception.hpp
  ${LIBLAS_HEADERS_DIR}/guid.hpp
  ${LIBLAS_HEADERS_DIR}/iterator.hpp
  ${LIBLAS_HEADERS_DIR}/lasbounds.hpp
  ${LIBLAS_HEADERS_DIR}/lasclassification.hpp
  ${LIBLAS_HEADERS_DIR}/lascolor.hpp
  ${LIBLAS_HEADERS_DIR}/laserror.hpp
  ${LIBLAS_HEADERS_DIR}/lasfilter.hpp
  ${LIBLAS_HEADERS_DIR}/lasheader.hpp
  ${LIBLAS_HEADERS_DIR}/laspoint.hpp
  ${LIBLAS_HEADERS_DIR}/lasreader.hpp
  ${LIBLAS_HEADERS_DIR}/lasschema.hpp
  ${LIBLAS_HEADERS_DIR}/lasspatialreference.hpp
  ${LIBLAS_HEADERS_DIR}/lastransform.hpp  
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
  lasbounds.cpp
  lasclassification.cpp
  lascolor.cpp
  laserror.cpp
  lasfilter.cpp
  lasheader.cpp
  laspoint.cpp
  lasreader.cpp
  lasspatialreference.cpp
  lasschema.cpp
  lastransform.cpp
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

if (APPLE)
    set_target_properties(
    ${LIBLAS_C_LIB_NAME}
    PROPERTIES
    INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    
    )
endif(APPLE)

########
target_link_libraries(${LIBLAS_C_LIB_NAME}
  ${LIBLAS_LIB_NAME}
  ${TIFF_LIBRARY}
  ${GEOTIFF_LIBRARY}
  ${GDAL_LIBRARY}
  ${SPATIALINDEX_LIBRARY}
  ${LIBXML2_LIBRARIES})

###############################################################################
# Targets installation

install(TARGETS ${LIBLAS_LIB_NAME} ${LIBLAS_C_LIB_NAME}
  RUNTIME DESTINATION ${LIBLAS_BIN_DIR}
  LIBRARY DESTINATION ${LIBLAS_LIB_DIR}
  ARCHIVE DESTINATION ${LIBLAS_LIB_DIR})

install(DIRECTORY ${LIBLAS_HEADERS_DIR}
  DESTINATION ${LIBLAS_INCLUDE_DIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

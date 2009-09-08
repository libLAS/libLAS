# $Id$
#
# Build the libLAS library subproject
#

SET( LIBLAS_INCLUDE_DIR ../include/liblas )

SET( LIBLAS_HPP
    ../include/liblas/cstdint.hpp
    ../include/liblas/exception.hpp
    ../include/liblas/guid.hpp
    ../include/liblas/iterator.hpp
    ../include/liblas/lascolor.hpp
    ../include/liblas/laserror.hpp
    ../include/liblas/lasfile.hpp
    ../include/liblas/lasheader.hpp
    ../include/liblas/laspoint.hpp
    ../include/liblas/lasreader.hpp
	../include/liblas/lasspatialreference.hpp
    ../include/liblas/lasvariablerecord.hpp
    ../include/liblas/laswriter.hpp
    ../include/liblas/liblas.hpp
)

SET( LIBLAS_DETAIL_HPP
    ../include/liblas/detail/endian.hpp
    ../include/liblas/detail/file.hpp
    ../include/liblas/detail/fwd.hpp
    ../include/liblas/detail/reader.hpp
    ../include/liblas/detail/reader10.hpp
    ../include/liblas/detail/reader11.hpp
	../include/liblas/detail/reader12.hpp
    ../include/liblas/detail/sha1.hpp
    ../include/liblas/detail/sharedptr.hpp
    ../include/liblas/detail/timer.hpp
    ../include/liblas/detail/utility.hpp
    ../include/liblas/detail/writer.hpp
    ../include/liblas/detail/writer10.hpp
    ../include/liblas/detail/writer11.hpp
	../include/liblas/detail/writer12.hpp
)

SET( LIBLAS_CPP
    lasclassification.cpp
    lascolor.cpp
    laserror.cpp
    lasfile.cpp
    lasheader.cpp
    laspoint.cpp
    lasreader.cpp
    lasspatialreference.cpp
    lasvariablerecord.cpp
    laswriter.cpp
)
# TODO: add IF GDAL_FOUND += gt_wkt_srs.cp

SET( LIBLAS_DETAIL_CPP
    detail/file.cpp
    detail/reader.cpp
    detail/reader10.cpp
    detail/reader11.cpp
	detail/reader12.cpp
    detail/writer.cpp
    detail/writer10.cpp
    detail/writer11.cpp
	detail/writer12.cpp
)

SET( LIBLAS_SOURCES
    ${LIBLAS_HPP}
    ${LIBLAS_DETAIL_HPP}
    ${LIBLAS_CPP}
    ${LIBLAS_DETAIL_CPP}
)

SET( LIBLAS_C_SOURCES
    ../include/liblas/capi/las_config.h
    ../include/liblas/capi/las_version.h
    ../include/liblas/capi/liblas.h
    las_c_api.cpp
)

SOURCE_GROUP( "cmake" FILES CMakeLists.txt )
SOURCE_GROUP( "Header Files" FILES ${LIBLAS_HPP} ${LIBLAS_DETAIL_HPP} )
SOURCE_GROUP( "Source Files" FILES ${LIBLAS_CPP} ${LIBLAS_DETAIL_CPP} )

# Diable Visual C++ language extensions when building libLAS library
IF (WIN32)
    ADD_DEFINITIONS( /Za )
ENDIF (WIN32)

# Configure GeoTIFF support (optional)
IF (GEOTIFF_FOUND)
    INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} )
ELSE (GEOTIFF_FOUND)
    SET(GEOTIFF_LIBRARY "")
ENDIF (GEOTIFF_FOUND)

INCLUDE_DIRECTORIES(
    ../include
)

# Static library with libLAS C++ core
ADD_LIBRARY( ${LIBLAS_LIB_NAME} STATIC ${LIBLAS_SOURCES} )

# Shared library with libLAS C API
ADD_LIBRARY( ${LIBLAS_C_LIB_NAME} SHARED ${LIBLAS_C_SOURCES} )
TARGET_LINK_LIBRARIES( ${LIBLAS_C_LIB_NAME}
	${LIBLAS_LIB_NAME}
	${GEOTIFF_LIBRARY}
)


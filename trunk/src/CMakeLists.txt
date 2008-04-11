# $Id$
#
# Build the libLAS library subproject
#

SET( LIBLAS_HEADERS
    ../include/liblas/detail/endian.hpp
    ../include/liblas/detail/file.hpp
    ../include/liblas/detail/fwd.hpp
    ../include/liblas/detail/reader.hpp
    ../include/liblas/detail/reader10.hpp
    ../include/liblas/detail/reader11.hpp
    ../include/liblas/detail/sha1.hpp
    ../include/liblas/detail/sharedptr.hpp
    ../include/liblas/detail/timer.hpp
    ../include/liblas/detail/utility.hpp
    ../include/liblas/detail/writer.hpp
    ../include/liblas/detail/writer10.hpp
    ../include/liblas/detail/writer11.hpp
    ../include/liblas/cstdint.hpp
    ../include/liblas/guid.hpp
    ../include/liblas/iterator.hpp
    ../include/liblas/laserror.hpp
    ../include/liblas/lasfile.hpp
    ../include/liblas/lasheader.hpp
    ../include/liblas/laspoint.hpp
    ../include/liblas/lasrecordheader.hpp
    ../include/liblas/lasreader.hpp
    ../include/liblas/lasrecordheader.hpp
    ../include/liblas/laswriter.hpp
    ../include/liblas/liblas.hpp
)

SET( LIBLAS_SRC
    detail/reader.cpp
    detail/reader10.cpp
    detail/reader11.cpp
    detail/writer.cpp
    detail/writer10.cpp
    detail/writer11.cpp
    detail/file.cpp
    laserror.cpp
    laspoint.cpp
    lasheader.cpp
    lasrecordheader.cpp
    lasreader.cpp
    laswriter.cpp
    lasfile.cpp
    ${LIBLAS_HEADERS}
)

SET( LIBLAS_HEADERS
    ../include/liblas/cstdint.hpp
)

SOURCE_GROUP( XXX FILES include/liblas/cstdint.hpp )

# Diable Visual C++ language extensions when building libLAS library
IF (WIN32)
    ADD_DEFINITIONS( /Za )
ENDIF (WIN32)

INCLUDE_DIRECTORIES(
    ../include
)

ADD_LIBRARY( ${LIBLAS_LIB_NAME} ${LIBLAS_SRC} )



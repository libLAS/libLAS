# $Id$
#
# Build the libLAS library subproject
#
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
    lasreader.cpp
    laswriter.cpp
    lasfile.cpp
)

# Diable Visual C++ language extensions when building libLAS library
IF (WIN32)
    ADD_DEFINITIONS( /Za )
ENDIF (WIN32)

INCLUDE_DIRECTORIES(
    ../include
)

ADD_LIBRARY( ${LIBLAS_LIB_NAME} ${LIBLAS_SRC} )

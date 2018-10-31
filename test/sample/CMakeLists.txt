# $Id$
#
# Build sample programs
#
SET( SAMPLE_READ sample_read )
SET( SAMPLE_WRITE sample_write )
SET( SAMPLE_FILES sample_files )

INCLUDE_DIRECTORIES(
    ../../include
)

ADD_EXECUTABLE( ${SAMPLE_READ} read.cpp )
ADD_EXECUTABLE( ${SAMPLE_WRITE} write.cpp )
ADD_EXECUTABLE( ${SAMPLE_FILES} files.cpp )

TARGET_LINK_LIBRARIES( ${SAMPLE_READ} las )
TARGET_LINK_LIBRARIES( ${SAMPLE_WRITE} las )
TARGET_LINK_LIBRARIES( ${SAMPLE_FILES} las )

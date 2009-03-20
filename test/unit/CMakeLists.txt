# $Id$
#
# Build libLAS unit test suite
#
SET( LIBLAS_TEST liblas_test )

SET( LIBLAS_TEST_SRC
    common.cpp
    guid_test.cpp
    laserror_test.cpp
    lasfile_test.cpp
    lasheader_test.cpp
    laspoint_test.cpp
    lasreader_iterator_test.cpp
    lasreader_test.cpp
    lasvariablerecord_test.cpp
    laswriter_test.cpp
    liblas_test_suite.cpp
)

INCLUDE_DIRECTORIES(
    .
    ../../include
)

ADD_EXECUTABLE( ${LIBLAS_TEST} ${LIBLAS_TEST_SRC} )

TARGET_LINK_LIBRARIES( ${LIBLAS_TEST} ${LIBLAS_LIB_NAME} )

# TODO: Needs to tweak the executable path
ADD_TEST(liblas_test ${EXECUTABLE_OUTPUT_PATH}/liblas_test)

# $Id$
#
# Build libLAS unit test suite
#
SET( LIBLAS_TEST liblas_test )

SET( LIBLAS_TEST_SRC
    guid_test.cpp
    laspoint_test.cpp
    lasheader_test.cpp
    lasrecordheader_test.cpp
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

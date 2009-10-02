###############################################################################
#
# test/unit/CMakeLists.txt controls building of libLAS unit tests suite
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################
SET(LIBLAS_UNIT_TEST liblas_test)

SET(LIBLAS_UNIT_TEST_SRC
    common.cpp
    guid_test.cpp
    laserror_test.cpp
    lasfile_test.cpp
    lasheader_test.cpp
    laspoint_test.cpp
    lasreader_iterator_test.cpp
    lasreader_test.cpp
    lasspatialreference_test.cpp
    lasvariablerecord_test.cpp
    laswriter_test.cpp
    liblas_test_suite.cpp)

INCLUDE_DIRECTORIES(
    .
    ../../include
    ${GDAL_INCLUDE_DIR}
    ${GEOTIFF_INCLUDE_DIR})

ADD_EXECUTABLE(${LIBLAS_UNIT_TEST} ${LIBLAS_UNIT_TEST_SRC})

TARGET_LINK_LIBRARIES(${LIBLAS_UNIT_TEST} 
    ${LIBLAS_LIB_NAME}
	${GDAL_LIBRARY}
	${GEOTIFF_LIBRARY})

# TODO: Needs to tweak the executable path
ADD_TEST(liblas_test ${EXECUTABLE_OUTPUT_PATH}/liblas_test ${CMAKE_SOURCE_DIR}/test/data)

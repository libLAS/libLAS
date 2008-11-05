INCLUDES = -I. -I../../include

TESTS = liblas_test

check_PROGRAMS = liblas_test

liblas_test_LDADD = \
	$(top_builddir)/src/liblas.la \
	$(NULL)

liblas_test_SOURCES = \
	common.cpp \
	guid_test.cpp \
	laserror_test.cpp \
	lasfile_test.cpp \
	lasheader_test.cpp \
	laspoint_test.cpp \
	lasreader_iterator_test.cpp \
	lasreader_test.cpp \
	lasrecordheader_test.cpp \
	laswriter_test.cpp \
	liblas_test_suite.cpp

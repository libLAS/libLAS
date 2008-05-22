
SUBDIRS = detail .

INCLUDES = -I../include -I../include/detail

if GEOTIFF_IS_CONFIG
AM_CPPFLAGS=@GEOTIFF_INC@ -DHAVE_LIBGEOTIFF=1
endif


lib_LTLIBRARIES = liblas.la

liblas_la_SOURCES = \
	laserror.cpp \
	laspoint.cpp \
	lasheader.cpp \
	lasrecordheader.cpp \
	lasreader.cpp \
	laswriter.cpp \
	lasfile.cpp \
	las_c_api.cpp

liblas_la_LIBADD = 	detail/liblasdetail.la

liblas_la_LDFLAGS = -version-info 1:0:0

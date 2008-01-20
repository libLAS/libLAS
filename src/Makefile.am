
SUBDIRS = detail .

INCLUDES = -I../include -I../include/detail

lib_LTLIBRARIES = liblas.la

liblas_la_SOURCES = lasheader.cpp lasreader.cpp las_c_api.cpp laspoint.cpp laserror.cpp
liblas_la_LIBADD = 	detail/liblasdetail.la

liblas_la_LDFLAGS = -version-info 1:0:0

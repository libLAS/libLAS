AM_CFLAGS = @GDAL_INC@ -I../include/liblas/capi -I../include
AM_CXXFLAGS = @GDAL_INC@ -I../include

lasinfo_SOURCES = lasinfo.c lascommon.c
las2las_SOURCES = las2las.c lascommon.c
lasmerge_SOURCES = lasmerge.c lascommon.c
las2txt_SOURCES = las2txt.c lascommon.c
txt2las_SOURCES = txt2las.c

bin_PROGRAMS = lasinfo las2las lasmerge las2txt txt2las

if GDAL_IS_CONFIG
las2ogr_SOURCES = las2ogr.cpp
bin_PROGRAMS += las2ogr
endif

LDADD = ../src/liblas.la

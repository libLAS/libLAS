INCLUDES = -I../include/liblas/capi -I../include

lasinfo_SOURCES = lasinfo.c lascommon.c
las2las_SOURCES = las2las.c
lasmerge_SOURCES = lasmerge.c lascommon.c
las2txt_SOURCES = las2txt.c lascommon.c
txt2las_SOURCES = txt2las.c

#if GDAL_IS_CONFIG
#las2ogr_SOURCES = las2ogr.cpp
#else
#las2ogr_SOURCES = 
#endif
LDADD = ../src/liblas.la
bin_PROGRAMS = lasinfo las2las lasmerge las2txt txt2las

INCLUDES = -I../include/liblas/capi -I../include

lasinfo_SOURCES = lasinfo.c lascommon.c
las2las_SOURCES = las2las.c lascommon.c
lasmerge_SOURCES = lasmerge.c lascommon.c
las2txt_SOURCES = las2txt.c lascommon.c
txt2las_SOURCES = txt2las.c

if GDAL_IS_CONFIG
las2ogr_SOURCES = las2ogr.cpp
bin_PROGRAMS = lasinfo las2las lasmerge las2txt txt2las las2ogr
else
bin_PROGRAMS = lasinfo las2las lasmerge las2txt txt2las

endif

LDADD = ../src/liblas.la

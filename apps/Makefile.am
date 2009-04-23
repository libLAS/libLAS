if GDAL_IS_CONFIG
GDAL_CPPFLAGS = @GDAL_INC@ -DHAVE_GDAL=1
endif

if GEOTIFF_IS_CONFIG
GEOTIFF_CPPFLAGS = @GEOTIFF_INC@ -DHAVE_LIBGEOTIFF=1
endif

if SPATIALINDEX_IS_CONFIG
SPATIALINDEX_CPPFLAGS = @SPATIALINDEX_INC@/spatialindex @SPATIALINDEX_INC@ -DHAVE_SPATIALINDEX=1
endif

AM_CPPFLAGS = $(GEOTIFF_CPPFLAGS) $(GDAL_CPPFLAGS) $(SPATIALINDEX_CPPFLAGS) -I../include/liblas/capi -I../include

lasinfo_SOURCES = lasinfo.c lascommon.c
las2las_SOURCES = las2las.c lascommon.c
lasmerge_SOURCES = lasmerge.c lascommon.c
las2txt_SOURCES = las2txt.c lascommon.c
txt2las_SOURCES = txt2las.c
ts2las_SOURCES = ts2las.cpp
lasindex_SOURCES = lasindex.cpp


bin_PROGRAMS = lasinfo las2las lasmerge las2txt txt2las ts2las 

if SPATIALINDEX_IS_CONFIG
bin_PROGRAMS += lasindex
endif

bin_SCRIPTS = liblas-config

if GDAL_IS_CONFIG
las2ogr_SOURCES = las2ogr.cpp
bin_PROGRAMS += las2ogr
endif

LDADD = ../src/liblas.la

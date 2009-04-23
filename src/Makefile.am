
SUBDIRS = .

if GDAL_IS_CONFIG
GDAL_CPPFLAGS = @GDAL_INC@ -DHAVE_GDAL=1
endif

if GEOTIFF_IS_CONFIG
GEOTIFF_CPPFLAGS = @GEOTIFF_INC@ -DHAVE_LIBGEOTIFF=1
endif

if SPATIALINDEX_IS_CONFIG
SPATIALINDEX_CPPFLAGS = @SPATIALINDEX_INC@/spatialindex @SPATIALINDEX_INC@ -DHAVE_SPATIALINDEX=1
endif

AM_CPPFLAGS = -I../include -I../include/detail $(GEOTIFF_CPPFLAGS) $(GDAL_CPPFLAGS) $(SPATIALINDEX_CPPFLAGS) 

lib_LTLIBRARIES = liblas.la

liblas_la_SOURCES = \
    lasclassification.cpp \
    lascolor.cpp \
	laserror.cpp \
	laspoint.cpp \
	lasheader.cpp \
	lasvariablerecord.cpp \
	lasreader.cpp \
	laswriter.cpp \
	lasfile.cpp \
	las_c_api.cpp \
	lasspatialreference.cpp \
    detail/reader.cpp \
    detail/reader10.cpp \
    detail/reader11.cpp \
    detail/reader12.cpp \
    detail/writer.cpp \
    detail/writer10.cpp \
    detail/writer11.cpp \
	detail/writer12.cpp \
    detail/file.cpp

if GDAL_IS_CONFIG
liblas_la_SOURCES += gt_wkt_srs.cpp
endif

if SPATIALINDEX_IS_CONFIG
liblas_la_SOURCES += lasindex.cpp 
endif
liblas_la_LDFLAGS = -version-info 1:0:0

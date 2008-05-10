#if GEOTIFF_IS_CONFIG
EXTRA_DEFS = -DHAVE_LIBGEOTIFF
#else
EXTRA_DEFS = 
#endif

INCLUDES = -I../../include -I../../include/detail @GEOTIFF_INC@ $(EXTRA_DEFS)

noinst_LTLIBRARIES = liblasdetail.la
liblasdetail_la_SOURCES = reader.cpp reader10.cpp reader11.cpp writer.cpp writer10.cpp writer11.cpp file.cpp

INCLUDES = -I../../include -I../../include/detail

if GEOTIFF_IS_CONFIG
AM_CPPFLAGS=@GEOTIFF_INC@ -DHAVE_LIBGEOTIFF=1
endif

noinst_LIBRARIES = liblasdetail.a
liblasdetail_a_SOURCES = reader.cpp reader10.cpp reader11.cpp writer.cpp writer10.cpp writer11.cpp file.cpp

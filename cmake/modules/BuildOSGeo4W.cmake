###############################################################################
#
# OSGeo4W packaging
#
# On success, the macro sets the following variables:
# GEOTIFF_FOUND       = if the library found
# GEOTIFF_LIBRARIES   = full path to the library
# GEOTIFF_INCLUDE_DIR = where to find the library headers also defined,
#                       but not for general use are
# GEOTIFF_LIBRARY     = where to find the PROJ.4 library.
# GEOTIFF_VERSION     = version of library which was found, e.g. "1.2.5"
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
# Module source: http://github.com/mloskot/workshop/tree/master/cmake/
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
###############################################################################

if(WIN32)
    set(DEFAULT_LIB_SUBDIR lib)
    set(DEFAULT_DATA_SUBDIR .)
    set(DEFAULT_INCLUDE_SUBDIR include)

    if (MSVC)
        set(DEFAULT_BIN_SUBDIR bin)
    else()
        set(DEFAULT_BIN_SUBDIR .)
    endif()
else()
    # Common locatoins for Unix and Mac OS X
    set(DEFAULT_BIN_SUBDIR bin)
    set(DEFAULT_LIB_SUBDIR lib)
    set(DEFAULT_DATA_SUBDIR share/liblas)
    set(DEFAULT_INCLUDE_SUBDIR include)
endif()

 ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/README.txt
   COMMAND ${CMAKE_COMMAND} xcopy /y /r/d /f ${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_SOURCE_DIR}/README.txt
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/README
   )

ADD_CUSTOM_TARGET(generate_readme DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/README.txt)
# CMake module to search for GeoTIFF library
#
# If it's found it sets GEOTIFF_FOUND to TRUE
# and following variables are set:
#    GEOTIFF_INCLUDE_DIR
#    GEOTIFF_LIBRARY
#
#
# FIND_PATH and FIND_LIBRARY normally search standard locations
# before the specified paths. To search non-standard paths first,
# FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
# and then again with no specified paths to search the default
# locations. When an earlier FIND_* succeeds, subsequent FIND_*s
# searching for the same item do nothing. 
FIND_PATH(GEOTIFF_INCLUDE_DIR geotiff.h
  "$ENV{LIB_DIR}/include"
  "$ENV{LIB_DIR}/include/geotiff"
  #mingw
  c:/msys/local/include
  NO_DEFAULT_PATH
  )
FIND_PATH(GEOTIFF_INCLUDE_DIR geotiff.h)

SET(GEOTIFF_NAMES ${GEOTIFF_NAMES} geotiff geotiff_i)

FIND_LIBRARY(GEOTIFF_LIBRARY NAMES ${GEOTIFF_NAMES} PATHS
  "$ENV{LIB_DIR}/lib"
  #mingw
  c:/msys/local/lib
  NO_DEFAULT_PATH
  )
FIND_LIBRARY(GEOTIFF_LIBRARY NAMES ${GEOTIFF_NAMES})

IF (GEOTIFF_INCLUDE_DIR AND GEOTIFF_LIBRARY)
   SET(GEOTIFF_FOUND TRUE)
ENDIF (GEOTIFF_INCLUDE_DIR AND GEOTIFF_LIBRARY)

IF (GEOTIFF_FOUND)

   IF (NOT GEOTIFF_FIND_QUIETLY)
      MESSAGE(STATUS "Found GeoTIFF: ${GEOTIFF_LIBRARY}")
   ENDIF (NOT GEOTIFF_FIND_QUIETLY)

ELSE (GEOTIFF_FOUND)

   IF (GEOTIFF_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find GeoTIFF library")
   ENDIF (GEOTIFF_FIND_REQUIRED)

ENDIF (GEOTIFF_FOUND)

###############################################################################
#
# OSGeo4W packaging
#
###############################################################################

MESSAGE(STATUS "   Building osgeo4w binaries")
set(OSGEO4W_DIR osgeo4w)
set(OSGEO4W_LIB_DIR ${OSGEO4W_DIR}/lib)
set(OSGEO4W_LIB_BIN_DIR ${OSGEO4W_DIR}/lib/bin)
set(OSGEO4W_DEVEL_DIR ${OSGEO4W_DIR}/devel)
set(OSGEO4W_DEVEL_INCLUDE_DIR ${OSGEO4W_DEVEL_DIR}/include)
set(OSGEO4W_DEVEL_INCLUDE_LIBLAS_DIR ${OSGEO4W_DEVEL_INCLUDE_DIR}/liblas)
set(OSGEO4W_DEVEL_LIB_DIR ${OSGEO4W_DEVEL_DIR}/lib)
set(OSGEO4W_PYTHON_DIR ${OSGEO4W_DIR}/apps/python25/lib/site-packages/liblas)



set(OSGEO4W_DIRECTORIES
    ${OSGEO4W_DIR}
    ${OSGEO4W_LIB_DIR}
    ${OSGEO4W_LIB_BIN_DIR}
    ${OSGEO4W_DEVEL_DIR}
    ${OSGEO4W_DEVEL_INCLUDE_DIR}
    ${OSGEO4W_DEVEL_INCLUDE_LIBLAS_DIR}
    ${OSGEO4W_DEVEL_LIB_DIR}
    ${OSGEO4W_PYTHON_DIR})



add_custom_target(BuildOSGeo4WDirectories
  COMMAND ${CMAKE_COMMAND} -E echo "Building OSGeo4W install directories")

macro (MAKE_OSGEO4W_DIRECTORIES)

   foreach(directory ${OSGEO4W_DIRECTORIES})

    STRING(REGEX REPLACE "/" "_" target "${directory}" )

    add_custom_target(${target}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${directory} )

      add_dependencies( BuildOSGeo4WDirectories ${target} )
   endforeach()


endmacro(MAKE_OSGEO4W_DIRECTORIES)

if (APPLE)
    set(SO_EXT dylib)
elseif(WIN32)
    set(SO_EXT dll)
else()
    set(SO_EXT so)
endif(APPLE)

set(OSGEO4W_FILES
    ${LIBLAS_BUILD_OUTPUT_DIRECTORY};${OSGEO4W_LIB_BIN_DIR}
    ${libLAS_SOURCE_DIR};${OSGEO4W_DEVEL_INCLUDE_LIBLAS_DIR}
    )

add_custom_target(CopyOSGeo4WFiles
  COMMAND ${CMAKE_COMMAND} -E echo "Copying OSGeo4W files...")

add_dependencies( CopyOSGeo4WFiles BuildOSGeo4WDirectories   )

macro (COPY_OSGEO4W_DIRECTORIES source destination)

   message(STATUS "Copying ${source} to ${destination}...")
   
    STRING(REGEX REPLACE "/" "_" target "${destination}" )

    add_custom_target(copy_${target}
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${source} ${destination})

      add_dependencies( CopyOSGeo4WFiles copy_${target} )


endmacro(COPY_OSGEO4W_DIRECTORIES source destination)    

MAKE_OSGEO4W_DIRECTORIES()
COPY_OSGEO4W_DIRECTORIES(${LIBLAS_BUILD_OUTPUT_DIRECTORY}/ ${OSGEO4W_LIB_BIN_DIR}/)
COPY_OSGEO4W_DIRECTORIES(${libLAS_SOURCE_DIR}/include/liblas ${OSGEO4W_DEVEL_INCLUDE_LIBLAS_DIR}/)
COPY_OSGEO4W_DIRECTORIES(${libLAS_SOURCE_DIR}/python/liblas/*.py ${OSGEO4W_PYTHON_DIR}/)


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


MAKE_OSGEO4W_DIRECTORIES()

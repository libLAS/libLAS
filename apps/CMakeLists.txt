###############################################################################
#
# apps/CMakeLists.txt controls building of libLAS utilities
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################

include_directories(
    .
    ../include
    ../include/liblas/capi)

###############################################################################
# Collect programs to build

set(LASINFO lasinfo)
set(LAS2LAS las2las)
set(LAS2TXT las2txt)
set(TXT2LAS txt2las)
set(TS2LAS ts2las)
set(LASBLOCK lasblock )

if (NOT WIN32)
    set(LAS2COL las2col )
    set(LAS2PG las2pg )
endif()

set(BIGFILE_TEST bigfile_test)
set(MEMLEAK_TEST memleak_test)
set(LASINDEX_TEST lasindex_test)

if(Boost_IOSTREAMS_FOUND)
  set(BIGFILE_BIO_TEST bigfile_boost_iostreams_test)
endif()

# Set the build type to release if it is not explicitly set by the user and
# isn't in the cache yet
if (NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Utilities depending on 3rd-pary libraries
if(GDAL_FOUND)
    set(LAS2OGR las2ogr)
endif()


set(LIBLAS_UTILITIES
    ${LASINFO} ${LAS2LAS} ${TXT2LAS}  ${LAS2COL}  ${LAS2PG}
    ${LAS2OGR} ${LAS2LAS} ${LASBLOCK} ${TS2LAS}  ${LAS2TXT})

# TODO: Experimental and requires testing --mloskot
# Generate user-specific settings for Visual Studio project
set(VCPROJ_USER_REMOTE_MACHINE_DEBUG ${MACHINE_NAME})
set(VCPROJ_USER_ENVIRONMENT_DEBUG "${ENVIRONMENT_PATH}")

if(MSVC)
    foreach(utility ${LIBLAS_UTILITIES})
        set(USER_FILE ${utility}.vcproj.$ENV{USERDOMAIN}.$ENV{USERNAME}.user)
        set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})
        message(STATUS "Generating ${CMAKE_GENERATOR} user-specific settings in ${USER_FILE}")
        configure_file(${CMAKE_SOURCE_DIR}/cmake/libLAS.vcproj.user.template ${OUTPUT_PATH} @ONLY)
    endforeach()
endif()

###############################################################################
# Configure build targets

if(WIN32)
    add_definitions("-DLAS_DLL_EXPORT=1")
endif()


set(APPS_CPP_DEPENDENCIES
    las
)

# link_directories(${Boost_LIBRARY_DIRS})

#    add_executable(lasschematest lasschematest.cpp laskernel.cpp)
#    target_link_libraries(lasschematest ${APPS_CPP_DEPENDENCIES} ${LIBXML2_LIBRARIES})


if(LAS2LAS)
    add_executable(${LAS2LAS} las2las.cpp laskernel.cpp)
    target_link_libraries(${LAS2LAS} ${APPS_CPP_DEPENDENCIES} )
endif()

if(LASINFO)
    add_executable(${LASINFO} lasinfo.cpp laskernel.cpp )
    target_link_libraries(${LASINFO} ${APPS_CPP_DEPENDENCIES}  )
endif()

if(LAS2TXT)
    add_executable( ${LAS2TXT}  las2txt.cpp laskernel.cpp )
    target_link_libraries(${LAS2TXT} ${APPS_CPP_DEPENDENCIES}  )
endif()

# Build txt2las
if(TXT2LAS)
    set(TXT2LAS_SRC lascommon.c ${TXT2LAS}.c)
    add_executable(${TXT2LAS} ${TXT2LAS_SRC})
    target_link_libraries(${TXT2LAS} las_c)
endif()

# Build las2col
if(LAS2COL)
    set(LAS2COL_SRC lascommon.c ${LAS2COL}.c)
    add_executable(${LAS2COL} ${LAS2COL_SRC})
    if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)
        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads REQUIRED)
        target_link_libraries(${LAS2COL} las_c Threads::Threads)
    else (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)
        find_package(Threads REQUIRED)
        target_link_libraries(${LAS2COL} las_c "${CMAKE_THREAD_LIBS_INIT}")
    endif (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)
endif()

# Build las2pg
if(LAS2PG)
    set(LAS2PG_SRC lascommon.c ${LAS2PG}.c)
    add_executable(${LAS2PG} ${LAS2PG_SRC})
    target_link_libraries(${LAS2PG} las_c)
endif()

if(TS2LAS)
    add_executable(${TS2LAS} ts2las.cpp laskernel.cpp)
    target_link_libraries(${TS2LAS} ${APPS_CPP_DEPENDENCIES} )
endif()

# Build lasblock
if(LASBLOCK)
    set(LASBLOCK_SRC lasblock.cpp laskernel.cpp)
    add_executable(${LASBLOCK} ${LASBLOCK_SRC})
    target_link_libraries(${LASBLOCK} ${APPS_CPP_DEPENDENCIES} )
endif()

# Build las2ogr
if(LAS2OGR)
    add_executable(${LAS2OGR} las2ogr.cpp)
    target_link_libraries(${LAS2OGR} ${APPS_CPP_DEPENDENCIES})
endif()


if(BIGFILE_TEST)
    add_executable(${BIGFILE_TEST} bigtest.c)
    target_link_libraries(${BIGFILE_TEST} las_c)
endif()

if(MEMLEAK_TEST)
    add_executable(${MEMLEAK_TEST} memleak_test.c)
    target_link_libraries(${MEMLEAK_TEST} las_c)
endif()


if (LASINDEX_TEST)
    add_executable(${LASINDEX_TEST} lasindex_test.cpp)
    target_link_libraries(${LASINDEX_TEST} ${APPS_CPP_DEPENDENCIES})
endif()

if(BIGFILE_BIO_TEST)
    add_executable(${BIGFILE_BIO_TEST} bigfile_boost_iostreams_test.cpp)
    target_link_libraries(${BIGFILE_BIO_TEST} ${APPS_CPP_DEPENDENCIES} )
endif()

###############################################################################
# Targets installation

install(TARGETS ${LIBLAS_UTILITIES}
    RUNTIME DESTINATION ${LIBLAS_BIN_DIR}
    LIBRARY DESTINATION ${LIBLAS_LIB_DIR}
    ARCHIVE DESTINATION ${LIBLAS_LIB_DIR})


if(UNIX)

  set(LIBLAS_UTILS_RPATH ${CMAKE_INSTALL_PREFIX}/lib ${Boost_LIBRARY_DIRS})
  if(LASZIP_FOUND)
    get_filename_component(LASZIP_LIBRARY_DIRS ${LASZIP_LIBRARY} PATH)
    set (LIBLAS_UTILS_RPATH ${LIBLAS_UTILS_RPATH} ${LASZIP_LIBRARY_DIRS})
  endif()
  if(GEOTIFF_FOUND)
    get_filename_component(GEOTIFF_LIBRARY_DIRS ${GEOTIFF_LIBRARY} PATH)
    set (LIBLAS_UTILS_RPATH ${LIBLAS_UTILS_RPATH} ${GEOTIFF_LIBRARY_DIRS})
  endif()
  if(GDAL_FOUND)
    get_filename_component(GDAL_LIBRARY_DIRS ${GDAL_LIBRARY} PATH)
    set (LIBLAS_UTILS_RPATH ${LIBLAS_UTILS_RPATH} ${GDAL_LIBRARY_DIRS})
  endif()
  set_target_properties(${LIBLAS_UTILITIES} PROPERTIES
    INSTALL_RPATH "${LIBLAS_UTILS_RPATH}")

  if(WITH_PKGCONFIG)

    set(PKGCFG_PREFIX "${CMAKE_INSTALL_PREFIX}")
    set(PKGCFG_INC_DIR "${LIBLAS_INCLUDE_SUBDIR}")
    set(PKGCFG_LIB_DIR "${LIBLAS_LIB_SUBDIR}")
    set(PKGCFG_REQUIRES  "")
    set(PKGCFG_VERSION ${VERSION})
    set(PKGCFG_LINK_FLAGS "-llas -llas_c")
    set(PKGCFG_COMPILE_FLAGS "")
    if(LIBXML2_FOUND)
      set(PKGCFG_REQUIRES "${PKGCFG_REQUIRES} libxml-2.0")
    endif()
    if(GEOTIFF_FOUND)
      set(PKGCFG_REQUIRES "${PKGCFG_REQUIRES} geotiff")
    endif()
    #  if(WITH_GDAL)
    #    set(PKGCFG_INC_DIR "${PKGCFG_INC_DIR} ${GDAL_INCLUDE_DIR}")
    #  endif()
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/liblas.pc.in
      ${CMAKE_CURRENT_BINARY_DIR}/liblas.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/liblas.pc
      DESTINATION ${LIBLAS_LIB_DIR}/pkgconfig
      PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

  else()

    # Autoconf compatibility variables to use the same script source.
    set(prefix ${CMAKE_INSTALL_PREFIX})
    set(exec_prefix ${CMAKE_INSTALL_PREFIX}/bin)
    set(libdir ${CMAKE_INSTALL_PREFIX}/lib)

    GET_DIRECTORY_PROPERTY(LIBLAS_DEFINITIONS DIRECTORY ${libLAS_SOURCE_DIR}/ COMPILE_DEFINITIONS)

    set(LIBLAS_CONFIG_DEFINITIONS "")
    foreach(definition ${LIBLAS_DEFINITIONS})
        set(LIBLAS_CONFIG_DEFINITIONS "${LIBLAS_CONFIG_DEFINITIONS} -D${definition}")
    endforeach()


    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/liblas-config.in
      ${CMAKE_CURRENT_BINARY_DIR}/liblas-config @ONLY)

    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/liblas-config
      DESTINATION bin)

  endif()

endif()

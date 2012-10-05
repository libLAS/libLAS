#############################################################################
# Config file generation and installation
#############################################################################

# Set where cmake will install liblas-config.cmake.  It's installed in
# ${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR} and ${PROJECT_ROOT_DIR}
# is the relative path to the root from there.
if (NOT WIN32)
  set(INSTALL_CMAKE_DIR "share/cmake/${PROJECT_NAME}-${VERSION}")
  set (PROJECT_ROOT_DIR "../../..")
else ()
  set(INSTALL_CMAKE_DIR "cmake")
  set (PROJECT_ROOT_DIR "..")
endif ()

# Now create the liblas-config files using the .in templates
configure_file (liblas-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/liblas-config.cmake @ONLY)
configure_file (liblas-config-version.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/liblas-config-version.cmake @ONLY)

# Install the liblas-config files so that other modules can find this
# project using 'find_package(libLAS)'
install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/liblas-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/liblas-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}")

# Make information about libLAS available.  The depends target is
# defined in ../src/CMakeLists.txt
install (EXPORT depends
  FILE "liblas-depends.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}")

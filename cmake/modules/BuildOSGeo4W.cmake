###############################################################################
#
# OSGeo4W packaging
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
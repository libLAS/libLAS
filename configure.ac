dnl $Id$
dnl
dnl This is main autoconf bootstrap script of libLAS project.
dnl
m4_define([liblas_version_major], [0])
m4_define([liblas_version_minor], [9])
m4_define([liblas_version_micro], [7])
m4_define([liblas_version],
          [liblas_version_major.liblas_version_minor.liblas_version_micro])

AC_PREREQ([2.59])
AC_INIT([las], [liblas_version], [hobu.inc@gmail.com])
AC_CANONICAL_BUILD

RELEASE_VERSION=liblas_version

dnl Default compilation flags
debug_default="no"
CFLAGS="-Wall -Wno-long-long -pedantic $CFLAGS"
CXXFLAGS="-Wall -Wno-long-long -pedantic -std=c++98 $CXXFLAGS"

dnl Checks for programs.
AM_INIT_AUTOMAKE
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL


dnl Checks for header files.
AC_CHECK_HEADERS([string.h],, [AC_MSG_ERROR([cannot find string.h, bailing out])])
AC_CHECK_HEADERS([stdio.h],, [AC_MSG_ERROR([cannot find stdio.h, bailing out])])
AC_CHECK_HEADERS([stdlib.h],, [AC_MSG_ERROR([cannot find stdlib.h, bailing out])])

LIBS="$LIBS"

AC_ARG_ENABLE([debug], [  --enable-debug=[no/yes] turn on debugging [default=$debug_default]],, enable_debug=$debug_default)

if test "x$enable_debug" = "xyes"; then
	CFLAGS="$CFLAGS -g -DDEBUG"
	CXXFLAGS="$CXXFLAGS -g -DDEBUG"
	AC_MSG_RESULT(yes)
else
	CFLAGS="$CFLAGS -O3 -DNDEBUG"
	CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG"
	AC_MSG_RESULT(no)
fi

AC_DEFUN([LOC_MSG],[
echo "$1"
])

AC_DEFUN([AC_HAVE_LONG_LONG],
[
  AC_MSG_CHECKING([for 64bit integer type])

  echo 'int main() { long long off=0; }' >> conftest.c
  if test -z "`${CC} -o conftest conftest.c 2>&1`" ; then
    AC_DEFINE(HAVE_LONG_LONG, 1, [Define to 1, if your compiler supports long long data type])
    AC_MSG_RESULT([long long])
  else
    AC_MSG_RESULT([no])
  fi
  rm -f conftest*
])

dnl ###########################################################
dnl GDAL
dnl ###########################################################
AC_ARG_WITH(gdal,[  --with-gdal=ARG         Path to gdal-config],,)
AC_MSG_CHECKING([for GDAL])

GDAL_CONFIG="no"
if test "$with_gdal" = "no" -o "$with_gdal" = "" ; then
  AC_MSG_RESULT([no])
else
  if test "`basename xx/$with_gdal`" = "gdal-config" ; then
    AC_MSG_RESULT([GDAL enabled with provided gdal-config])
    GDAL_CONFIG="$with_gdal"
  else
    AC_MSG_ERROR([--with-gdal should have yes or a path to gdal-config])
  fi
fi
if test "$GDAL_CONFIG" != "no" ; then
  VERSION=`$GDAL_CONFIG --version`
  AC_MSG_RESULT([$GDAL_CONFIG reports version $VERSION])
  LIBS="`$GDAL_CONFIG --libs` $LIBS"
  GDAL_INC=`$GDAL_CONFIG --cflags`
  GDAL_PREFIX=`$GDAL_CONFIG --prefix`
  OGR=`$GDAL_CONFIG --ogr-enabled`
  if test "$OGR" != "yes" ; then
  	AC_MSG_ERROR([$GDAL_CONFIG says OGR is not enabled])
  fi
fi
AC_SUBST(GDAL_INC GDAL_PREFIX)
AM_CONDITIONAL([GDAL_IS_CONFIG], [test ! x$GDAL_CONFIG = xno])

dnl ---------------------------------------------------------------------------
dnl Select a libgeotiff library to use.
dnl ---------------------------------------------------------------------------

AC_ARG_WITH(geotiff,[  --with-geotiff=ARG    Libgeotiff library to use (yes or path)],,)
AC_MSG_CHECKING([for libgeotiff])

GEOTIFF_CONFIG="no"
if test "$with_geotiff" = "no" -o "$with_geotiff" = "" ; then

  AC_MSG_RESULT([no])

else

  GEOTIFF_CONFIG=external
  dnl We now require libgeotiff 1.2.1 (for XTIFFClientOpen). 
  dnl first check if $with_geotiff/lib has the library:
  AC_CHECK_LIB(geotiff,XTIFFClientOpen,GEOTIFF_CONFIG=external,GEOTIFF_CONFIG=no,-L$with_geotiff/lib)
fi

if test "$GEOTIFF_CONFIG" != "no" ; then

  if test $GEOTIFF_CONFIG = "external" ; then
    LIBS="-L$with_geotiff/lib -lgeotiff $LIBS"
    if test  -d $with_geotiff/include ; then
      GEOTIFF_INC="-I$with_geotiff/include"
      AC_MSG_RESULT([using libgeotiff from $with_geotiff/include]) 
    fi
  fi

fi

AC_SUBST(GEOTIFF_INC GEOTIFF_PREFIX)
AM_CONDITIONAL([GEOTIFF_IS_CONFIG], [test ! x$GEOTIFF_CONFIG = xno])

AC_HAVE_LONG_LONG

dnl Checks for library functions.
AC_CHECK_FUNCS([gettimeofday bzero memset memcpy bcopy])

AC_CONFIG_FILES([
    Makefile
    include/Makefile
    src/Makefile
    src/detail/Makefile
    apps/Makefile
    test/Makefile
    test/unit/Makefile
])

AC_OUTPUT

dnl -------------------------------------------------------------------
dnl Print configuration summary
dnl -------------------------------------------------------------------
LOC_MSG()
LOC_MSG([libLAS configuration summary:])
LOC_MSG()
LOC_MSG([  Version..................: ${RELEASE_VERSION}])
LOC_MSG([  Installation directory...: ${prefix}])
LOC_MSG([  C compiler...............: ${CC} ${CFLAGS}])
LOC_MSG([  C++ compiler.............: ${CXX} ${CXXFLAGS}])
LOC_MSG([  Debugging support........: ${enable_debug}])
LOC_MSG([  GDAL support.............: ${GDAL_CONFIG} ${GDAL_INC}])
LOC_MSG([  GeoTIFF SRS support......: ${GEOTIFF_CONFIG} ${GEOTIFF_INC}])
LOC_MSG()
LOC_MSG([  LIBS.....................: ${LIBS}])

LOC_MSG()

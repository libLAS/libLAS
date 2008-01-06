AC_PREREQ(2.59)
AC_INIT(las, 1.0.0, hobu.inc@gmail.com)
AC_CANONICAL_BUILD

debug_default="no"
CXXFLAGS="$CXXFLAGS"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE

CC=$(CXX)

# Checks for header files.
AC_CHECK_HEADERS(string.h,, [AC_MSG_ERROR([cannot find string.h, bailing out])])
AC_CHECK_HEADERS(stdio.h,, [AC_MSG_ERROR([cannot find stdio.h, bailing out])])
AC_CHECK_HEADERS(stdlib.h,, [AC_MSG_ERROR([cannot find stdlib.h, bailing out])])

LIBS="$LIBS"

AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging [default=$debug_default]],, enable_debug=$debug_default)

if test "x$enable_debug" = "xyes"; then
	CXXFLAGS="$CXXFLAGS -g -DDEBUG"
	AC_MSG_RESULT(yes)
else
	CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG"
	AC_MSG_RESULT(no)
fi

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

AC_HAVE_LONG_LONG

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday bzero memset memcpy bcopy])

AC_CONFIG_FILES([	Makefile
			include/Makefile
			src/Makefile
			apps/Makefile
		])

AC_OUTPUT


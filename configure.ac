AC_INIT([libblake2],[1],[mt-ml@gmx.de])
AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign subdir-objects silent-rules no-dist-gzip dist-xz tar-pax])
AM_SILENT_RULES([yes])
AC_CONFIG_MACRO_DIRS([m4])

AC_C_BIGENDIAN
if test "x$ac_cv_c_bigendian" = "xyes"; then
    AC_MSG_ERROR(big endian systems not supported)
fi

AC_PROG_CXX
AM_PROG_AR

AC_LANG_PUSH([C++])
AC_CHECK_HEADER([gtest/gtest.h],[],AC_MSG_ERROR(google test not found))
AC_LANG_POP([C++])

LT_INIT

AC_OUTPUT([Makefile])

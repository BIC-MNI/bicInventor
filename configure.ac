dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_SRCDIR([tools/bicobj2iv.cc])
AM_INIT_AUTOMAKE(bicInventor, 0.1)
AM_CONFIG_HEADER(config.h)

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_PROG_CC
AC_PROG_CXX

dnl Build only static libs by default
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

smr_WITH_BUILD_PATH

# Check for required C libraries
mni_REQUIRE_BICPL

# Check for rquired C++ libraries
AC_LANG_PUSH(C++)
mni_REQUIRE_LIB(oobicpl,[#include <mniVolume.h>],[mniVolume vol;])
mni_REQUIRE_SOQT

AC_CONFIG_FILES([Makefile lib/Makefile tools/Makefile])
AC_OUTPUT

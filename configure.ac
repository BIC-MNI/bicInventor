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

smr_REQUIRED_LIB(m,sqrt,math.h)
smr_REQUIRED_LIB(netcdf,ncopen,netcdf.h)
smr_REQUIRED_LIB(minc,miicv_create,minc.h)
smr_REQUIRED_LIB(volume_io,input_volume,volume_io.h)
smr_REQUIRED_LIB(bicpl,input_graphics_file,bicpl.h)

# I'd like to use smr_REQUIRED_LIB for Inventor, too.  But the library
# doesn't seem to define any global functions to test for.

AC_LANG_PUSH(C++)

AC_MSG_CHECKING([for -loobicpl])
LIBS="-loobicpl $LIBS"
AC_TRY_LINK([#include <mniVolume.h>],
            [mniVolume vol;],,
            AC_MSG_ERROR([cannot find oobicpl library]))
AC_MSG_RESULT([yes])

AC_MSG_CHECKING([for -lInventor])
LIBS="-lInventor $LIBS"
AC_TRY_LINK([#include <Inventor/SoDB.h>],
            [SoDB::init();],,
            AC_MSG_ERROR([cannot find Inventor library]))
AC_MSG_RESULT([yes])

AC_LANG_POP


AC_CONFIG_FILES([Makefile lib/Makefile tools/Makefile])
AC_OUTPUT

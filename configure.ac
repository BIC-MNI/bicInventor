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

AC_LANG_PUSH(C++)
# Function main() used because Inventor does not define any global functions
# This trick is allowed by GCC, but IRIX CC complains.
smr_REQUIRED_LIB(Inventor,main,Inventor/SoDB.h)
AC_LANG_POP


AC_CONFIG_FILES([Makefile lib/Makefile tools/Makefile])
AC_OUTPUT

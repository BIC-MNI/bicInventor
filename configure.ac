dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_SRCDIR([tools/bicobj2iv.cc])
AM_INIT_AUTOMAKE(bicpl++, 0.0)
AM_CONFIG_HEADER(config.h)

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_PROG_CC
AC_PROG_CXX

dnl Build only static libs by default
dnl AM_DISABLE_SHARED
dnl AM_PROG_LIBTOOL

smr_WITH_BUILD_PATH

smr_REQUIRED_LIB(m,sqrt,math.h)
smr_REQUIRED_LIB(netcdf,ncopen,netcdf.h)
smr_REQUIRED_LIB(minc,miicv_create,minc.h)
smr_REQUIRED_LIB(volume_io,input_volume,volume_io.h)
smr_REQUIRED_LIB(bicpl,input_graphics_file,bicpl.h)

AC_LANG_PUSH(C++)
# Function main() used because Inventor does not define any global functions
smr_OPTIONAL_LIB(Inventor,main,Inventor/SoDB.h)
AC_LANG_POP


AM_CONDITIONAL(HAVE_Inventor,[test $HAVE_Inventor = yes])


AC_CONFIG_FILES([Makefile tools/Makefile])
AC_OUTPUT

bin_PROGRAMS = $(Inventor_progs)

INCLUDES = -I$(top_srcdir)/inventor

ParseArgv_src = ParseArgv.c ParseArgv.h


if HAVE_libInventor
Inventor_progs = bicobj2iv
else
Inventor_progs =
endif




bicobj2iv_SOURCES = bicobj2iv.cc $(ParseArgv_src)
bicobj2iv_LDADD = ../inventor/libbicInventor.la -lInventor
bin_PROGRAMS = bicobj2iv mesh2iv meshmorph

INCLUDES = -I$(top_srcdir)/lib
LDADD = ../lib/libbicInventor.la

ParseArgv_src = ParseArgv.c ParseArgv.h

bicobj2iv_SOURCES = bicobj2iv.cc $(ParseArgv_src)
mesh2iv_SOURCES = mesh2iv.cc $(ParseArgv_src)
meshmorph_SOURCES = meshmorph.cc $(ParseArgv_src)
meshmorph_LDADD = $(LDADD) -lSoQt

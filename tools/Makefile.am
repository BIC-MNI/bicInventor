bin_PROGRAMS = $(Inventor_progs)

ParseArgv_src = ParseArgv.c ParseArgv.h


if HAVE_Inventor
Inventor_progs = bicobj2iv
else
Inventor_progs =
endif




bicobj2iv_SOURCES = bicobj2iv.cc $(ParseArgv_src)
bicobj2iv_LDADD = -lInventor
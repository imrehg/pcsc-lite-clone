# Process this file with automake to create Makefile.in.

lib_LTLIBRARIES = libmusclecard.la
libmusclecard_la_LDFLAGS = -version-info 1:0:0 -Wl,-Bsymbolic

noinst_PROGRAMS = muscletest

nobase_include_HEADERS = \
	PCSC/mscdefines.h \
	PCSC/musclecard.h
noinst_HEADERS = \
	debug.h \
	tokenfactory.h

libmusclecard_la_SOURCES = \
	debug.c \
	musclecard.c \
	tokenfactory.c \
	tokenparser.l \
	dyn_generic.h \
	dyn_hpux.c \
	dyn_macosx.c \
	dyn_unix.c \
	misc.h \
	parser.h \
	strlcpy.c \
	strlcpycat.h \
	strlcat.c \
	sys_generic.h \
	thread_generic.h \
	thread_unix.c
libmusclecard_la_CFLAGS = \
	-I$(top_srcdir)/src/PCSC \
	$(PCSCLITE_CFLAGS) \
	$(PTHREAD_CFLAGS) \
	$(SYMBOL_VISIBILITY)
libmusclecard_la_LIBADD = $(PCSCLITE_LIBS) $(LIBDL) \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

muscletest_SOURCES = \
	muscletest.c \
	strlcpy.c
muscletest_CFLAGS = \
	-I$(top_srcdir)/src/PCSC \
	$(PCSCLITE_CFLAGS)
muscletest_LDADD = \
	libmusclecard.la

pcdir= $(libdir)/pkgconfig
pc_DATA = libmusclecard.pc


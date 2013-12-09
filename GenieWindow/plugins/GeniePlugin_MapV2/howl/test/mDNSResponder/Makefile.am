SUBDIRS							=	Posix Win32
INCLUDES							=	-I$(top_srcdir)/include/ -I$(top_srcdir)/src/lib/howl -I$(top_srcdir)/src/lib/mDNSResponder
LDADD								=	$(top_builddir)/src/lib/mDNSResponder/libmDNSResponder.la $(top_builddir)/src/lib/howl/libhowl.la $(PLATFORM_LIBS)
bin_PROGRAMS					=	mDNSResponder
mDNSResponder_SOURCES		=	Posix/posix_main.c

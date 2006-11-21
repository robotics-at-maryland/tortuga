INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/OgreMain/include -I$(top_srcdir)/Samples/Common/include $(OIS_CFLAGS)

noinst_PROGRAMS = Dot3Bump

Dot3Bump_SOURCES = Dot3Bump.cpp
Dot3Bump_LDFLAGS = -L$(top_builddir)/OgreMain/src
Dot3Bump_LDADD = -lOgreMain $(OIS_LIBS)

install-exec-local:
	$(INSTALL_PROGRAM) .libs/Dot3Bump $(top_builddir)/Samples/Common/bin/Dot3Bump

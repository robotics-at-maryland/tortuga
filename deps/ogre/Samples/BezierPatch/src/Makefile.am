INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/Samples/BezierPatch/include -I$(top_srcdir)/OgreMain/include -I$(top_srcdir)/Samples/Common/include $(OIS_CFLAGS)

noinst_PROGRAMS = Bezier

Bezier_SOURCES = Bezier.cpp
Bezier_LDFLAGS = -L$(top_builddir)/OgreMain/src
Bezier_LDADD = -lOgreMain $(OIS_LIBS)

install-exec-local:
	$(INSTALL_PROGRAM) .libs/Bezier $(top_builddir)/Samples/Common/bin/Bezier

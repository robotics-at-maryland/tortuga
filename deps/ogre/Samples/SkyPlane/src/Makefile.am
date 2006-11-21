INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/Samples/SkyPlane/include -I$(top_srcdir)/OgreMain/include -I$(top_srcdir)/Samples/Common/include $(OIS_CFLAGS)

noinst_PROGRAMS = SkyPlane
SkyPlane_SOURCES = SkyPlane.cpp
SkyPlane_LDFLAGS = -L$(top_builddir)/OgreMain/src
SkyPlane_LDADD = -lOgreMain $(OIS_LIBS)

install-exec-local:
	$(INSTALL_PROGRAM) .libs/SkyPlane $(top_builddir)/Samples/Common/bin/SkyPlane

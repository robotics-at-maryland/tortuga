INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/OgreMain/include -I$(top_srcdir)/Samples/Common/include $(OIS_CFLAGS)

noinst_PROGRAMS = Grass

Grass_SOURCES = Grass.cpp
Grass_LDFLAGS = -L$(top_builddir)/OgreMain/src
Grass_LDADD = -lOgreMain $(OIS_LIBS)

install-exec-local:
	$(INSTALL_PROGRAM) .libs/Grass $(top_builddir)/Samples/Common/bin/Grass

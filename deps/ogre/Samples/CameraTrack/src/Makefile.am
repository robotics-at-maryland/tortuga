INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/Samples/CameraTrack/include -I$(top_srcdir)/OgreMain/include -I$(top_srcdir)/Samples/Common/include $(OIS_CFLAGS)

noinst_PROGRAMS = CameraTrack

CameraTrack_SOURCES = CameraTrack.cpp
CameraTrack_LDFLAGS = -L$(top_builddir)/OgreMain/src
CameraTrack_LDADD = -lOgreMain $(OIS_LIBS)

install-exec-local:
	$(INSTALL_PROGRAM) .libs/CameraTrack $(top_builddir)/Samples/Common/bin/CameraTrack

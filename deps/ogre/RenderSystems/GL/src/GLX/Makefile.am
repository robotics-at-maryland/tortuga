INCLUDES = $(STLPORT_CFLAGS) \
	   -I$(top_srcdir)/RenderSystems/GL/include \
	   -I$(top_srcdir)/RenderSystems/GL/include/GLX \
           -I$(top_srcdir)/OgreMain/include $(GLSUPPORT_CFLAGS)

noinst_LTLIBRARIES = libsupport.la

libsupport_la_SOURCES = OgreGLXGLSupport.cpp \
			OgreGLXWindow.cpp \
			OgreGLXRenderTexture.cpp \
			OgreGLXContext.cpp \
			OgreGLXUtils.cpp

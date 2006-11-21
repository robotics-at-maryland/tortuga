INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/includes $(CFLAGS) -I/usr/X11R6/include

lib_LTLIBRARIES=libOIS.la
libOIS_la_SOURCES = OISInputManager.cpp \
	OISObject.cpp \
	OISEffect.cpp \
	OISJoyStick.cpp \
	OISKeyboard.cpp \
	OISForceFeedback.cpp \
	./linux/EventHelpers.cpp \
	./linux/LinuxInputManager.cpp \
        ./linux/LinuxJoyStickEvents.cpp \
        ./linux/LinuxForceFeedback.cpp \
	./linux/LinuxKeyboard.cpp \
	./linux/LinuxMouse.cpp

libOIS_la_LDFLAGS = -release @PACKAGE_VERSION@
libOIS_la_LIBADD = $(STLPORT_LIBS) -L/usr/X11R6/lib -lX11 -lXaw
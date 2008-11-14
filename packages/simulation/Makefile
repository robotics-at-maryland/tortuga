OGRE_ROOT=/opt/ogre
CFLAGS=`PKG_CONFIG_PATH=$(OGRE_ROOT)/lib/pkgconfig pkg-config OGRE --cflags` -I$(RAM_SVN_DIR)/packages
LDFLAGS=`PKG_CONFIG_PATH=$(OGRE_ROOT)/lib/pkgconfig pkg-config OGRE OIS --libs`

all: main

main.o: src/main.cpp
	g++ $(CFLAGS) -c src/main.cpp

main: main.o
	g++ main.o -o main $(LDFLAGS)

clean:
	rm -rf *.o main

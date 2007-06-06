LIBFLAGS=-nostdlib -shared -fPIC -Wl,-soname,$@.1
INCLUDES=-I/Users/Dan/robot/packages

vision.x: main.cpp ../include/main.h ../include/ProcessList.h ProcessList.cpp
	g++ -g $(INCLUDES) -I/usr/local/include/opencv -o vision.x main.cpp ProcessList.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

vision.o: main.cpp ../include/main.h
	g++ -g $(INCLUDES) -I/usr/local/include/opencv -o vision.x main.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

vision.so: vision.o ../include/main.h
	gcc -o vision.so $(INCLUDES) $(LIBFLAGS) vision.o

hough.x: houghDemo.cpp houghDemo.h
	g++ -I/usr/local/include/opencv -o hough.x houghDemo.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

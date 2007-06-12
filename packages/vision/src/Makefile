LIBFLAGS=-bundle -flat_namespace -undefined suppress
INCLUDES=-I/Users/Dan/robot/packages

vision.x: main.cpp ../include/main.h ../include/ProcessList.h ProcessList.cpp
	g++ -g $(INCLUDES) -I/usr/local/include/opencv -o vision.x main.cpp ProcessList.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

vision.o: main.cpp ../include/main.h
	g++ -c -g $(INCLUDES) -I/usr/local/include/opencv -o vision.o main.cpp ProcessList.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

vision.so: vision.o ../include/main.h
	gcc $(LIBFLAGS) -o vision.so vision.o

hough.x: houghDemo.cpp houghDemo.h
	g++ -I/usr/local/include/opencv -o hough.x houghDemo.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

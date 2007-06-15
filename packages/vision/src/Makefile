LIBFLAGS=-bundle
INCLUDES=-I/Users/Dan/robot/packages -I/usr/local/include/opencv 

vision.x: main.cpp ../include/main.h ../include/ProcessList.h ProcessList.cpp
	g++ -g $(INCLUDES) -I/usr/local/include/opencv -o vision.x main.cpp ProcessList.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

vision.o: main.cpp ../include/main.h
	g++ -fPIC -c -g $(INCLUDES) -o vision.o main.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

ProcessList.o: ProcessList.cpp ../include/ProcessList.h
	g++ -fPIC -c -g $(INCLUDES) -o ProcessList.o ProcessList.cpp

VisionCommunication.o: VisionCommunication.cpp ../include/VisionCommunication.h ../include/VisionData.h
	g++ -fPIC -c -g $(INCLUDES) -o VisionCommunication.o VisionCommunication.cpp

vision.so: vision.o ProcessList.o VisionCommunication.o ../include/main.h
	g++ $(LIBFLAGS) -o vision.so vision.o ProcessList.o VisionCommunication.o -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

hough.x: houghDemo.cpp houghDemo.h
	g++ -I/usr/local/include/opencv -o hough.x houghDemo.cpp -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

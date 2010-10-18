OIS_ROOT=$(RAM_ROOT_DIR)
CFLAGS=-DLOGITECH
HEADERS=logitech.h joystick.h

all: oisserver oisclient keepalive


oisserver: oisserver.c

keepalive: keepalive.cpp

oisclient: oisclient.cpp $(HEADERS)
	g++ oisclient.cpp -lOIS $(CFLAGS) -L$(OIS_ROOT)/lib -I$(OIS_ROOT)/include/OIS/ -o oisclient -g

clean:
	rm -f oisclient oisserver *.o *~

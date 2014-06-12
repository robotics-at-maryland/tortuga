OIS_ROOT=/usr
HEADERS=logitech.h joystick.h

all: oisserver logitech keepalive

.PHONY: saitek logitech generic
saitek: JOYSTICK=SAITEK
logitech: JOYSTICK=LOGITECH
generic: JOYSTICK=GENERIC
saitek logitech generic: oisclient

oisserver: oisserver.c

keepalive: keepalive.cpp

oisclient: oisclient.cpp $(HEADERS)
	g++ oisclient.cpp -D$(JOYSTICK) -lOIS $(CFLAGS) -L$(OIS_ROOT)/lib -I$(OIS_ROOT)/include/OIS/ -o oisclient -g

clean:
	rm -f oisclient oisserver *.o *~

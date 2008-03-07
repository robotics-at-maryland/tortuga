OIS_ROOT=/usr
CFLAGS=-DSAITEK

all: oisserver oisclient keepalive


oisserver: oisserver.c

keepalive: keepalive.cpp

oisclient: oisclient.cpp
	g++ oisclient.cpp -lOIS $(CFLAGS) -L$(OIS_ROOT)/lib -I$(OIS_ROOT)/include/OIS/ -o oisclient -g

clean:
	rm -f oisclient oisserver *.o *~

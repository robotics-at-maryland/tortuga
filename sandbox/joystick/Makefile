OIS_ROOT=/usr
CFLAGS=-DSAITEK

all: oisserver oisclient


oisserver: oisserver.c


oisclient: oisclient.cpp
	g++ oisclient.cpp -lOIS $(CFLAGS) -L$(OIS_ROOT)/lib -I$(OIS_ROOT)/include/OIS/ -o oisclient -g

clean:
	rm -f oisclient oisserver *.o *~ 

all: oisserver oisclient


oisserver: oisserver.c


oisclient: oisclient.cpp
	g++ oisclient.cpp -lOIS -I /usr/include/OIS/ -o oisclient -g

clean:
	rm -f oisclient oisserver *.o *~ 

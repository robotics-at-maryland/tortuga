CFLAGS= -Iinclude -Wall

all: dvlcomm

clean:
	rm -rf *.o *~ dvl_api dvlcomm

dvlapi.o: src/dvlapi.c include/dvlapi.h
	gcc -c src/dvlapi.c -o dvlapi.o $(CFLAGS)

dvlcomm.o: dvlcomm.c include/dvlapi.h
	gcc -c dvlcomm.c -o dvlcomm.o $(CFLAGS)

dvlcomm: dvlcomm.o dvlapi.o include/dvlapi.h
	gcc dvl_api.o dvlcomm.o -o dvlcomm $(CFLAGS)


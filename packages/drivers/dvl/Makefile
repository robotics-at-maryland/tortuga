CFLAGS= -Iinclude -Wall -Werror -g

all: dvltest

clean:
	rm -rf *.o *~ dvl_api dvltest

dvlapi.o: src/dvlapi.c include/dvlapi.h
	gcc -c src/dvlapi.c -o dvlapi.o $(CFLAGS)

dvltest.o: dvltest.c include/dvlapi.h
	gcc -c dvltest.c -o dvltest.o $(CFLAGS)

dvltest: dvltest.o dvlapi.o include/dvlapi.h
	gcc dvlapi.o dvltest.o -o dvltest $(CFLAGS)


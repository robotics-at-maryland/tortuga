CC = gcc
LD = gcc
AR = ar

LDFLAGS= -lm -Lbin
CFLAGS= -g -Iinclude

all: bin/libimu.a imutest

clean:
	rm -f imutest *~ *.o bin

# Building IMU library 
bin:
	mkdir -p bin

bin/imuapi.o: src/imuapi.c include/imuapi.h bin
	$(CC) $(CFLAGS) -c src/imuapi.c -o bin/imuapi.o

bin/libimu.a: bin/imuapi.o
	$(AR) rcs bin/libimu.a bin/imuapi.o
	ranlib bin/libimu.a

# Test Program
bin/imutest.o: test/src/imutest.c include/imuapi.h
	$(CC) $(CFLAGS) -c test/src/imutest.c -o bin/imutest.o 

imutest: bin/libimu.a bin/imutest.o
	$(CC) $(LDFLAGS)  bin/imutest.o -limu -o imutest

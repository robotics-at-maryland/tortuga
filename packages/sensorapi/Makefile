all: sensortest lcdshow

clean:
	rm -rf *.o *~ sensortest lcdshow

sensorapi.o: sensorapi.c sensorapi.h

sensortest: sensorapi.o sensortest.o

lcdshow: lcdshow.o sensorapi.o sensorapi.h
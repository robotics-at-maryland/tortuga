all: sensortest lcdshow

clean:
	rm -rf *.o *~ sensortest lcdshow

sensorapi.o: src/sensorapi.c include/sensorapi.h
	gcc -c src/sensorapi.c -o sensorapi.o -Iinclude

sensortest.o: test/src/sensortest.c include/sensorapi.h
	gcc -c test/src/sensortest.c -o sensortest.o -Iinclude

lcdshow.o: lcdshow.c include/sensorapi.h
	gcc -c lcdshow.c -Iinclude

sensortest: sensorapi.o sensortest.o
	gcc sensorapi.o sensortest.o -o sensortest

lcdshow: lcdshow.o sensorapi.o include/sensorapi.h
	gcc lcdshow.o sensorapi.o -o sensortest
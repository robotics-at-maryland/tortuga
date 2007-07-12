all: src/thrusterapi.o thrustertest

thrustertest: thrusterapi.o thrustertest.o
	gcc thrusterapi.o thrustertest.o -o thrustertest

thrusterapi.o: src/thrusterapi.c include/thrusterapi.h
	gcc -c src/thrusterapi.c -o thrusterapi.o -Iinclude

thrustertest.o: test/src/thrustertest.c
	gcc -c test/src/thrustertest.c -o thrustertest.o -Iinclude

clean:
	rm -rf *.o thrustertest

$(CC) = gcc

imutest: imuapi.h imuapi.o imutest.o
	$(CC) imuapi.o imutest.o -lm -o imutest

clean:
	rm imutest *~ *.o -f

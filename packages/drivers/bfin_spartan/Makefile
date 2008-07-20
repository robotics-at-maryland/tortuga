PATH=/opt/uClinux/bfin-linux-uclibc/bin
CC = bfin-linux-uclibc-gcc
CFLAGS = -O3 -Iinclude -I../..

all:  readtest testreport adctest

adctest: adctest.c src/spartan.c include/spartan.h
sramtest: sramtest.c
amptest: amptest.c  src/spartan.c include/spartan.h
readtest: readtest.c  src/spartan.c include/spartan.h

testreport: src/report.c include/report.h testreport.c

clean:
	/bin/rm -rf adctest readtest sramtest testreport *~ *.o

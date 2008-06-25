PATH=/opt/uClinux/bfin-linux-uclibc/bin
CC = bfin-linux-uclibc-gcc
CFLAGS = -O3

all: adctest readtest sramtest chrt

adctest: adctest.c spartan.c spartan.h
sramtest: sramtest.c
amptest: amptest.c  spartan.c spartan.h
readtest: readtest.c  spartan.c spartan.h


clean:
	/bin/rm -rf adctest readtest sramtest *~ *.o

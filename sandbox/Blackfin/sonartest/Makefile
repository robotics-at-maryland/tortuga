PATH=/opt/uClinux/bfin-linux-uclibc/bin
CC = bfin-linux-uclibc-gcc
CFLAGS = -O3

all: adctest readtest sramtest chrt amptest

adctest: adctest.c spartan.c spartan.h
readtest: readtest.c  spartan.c spartan.h
sramtest: sramtest.c
amptest: spartan.c spartan.h amptest.c

clean:
	/bin/rm -rf adctest readtest sramtest amptest *~ *.o

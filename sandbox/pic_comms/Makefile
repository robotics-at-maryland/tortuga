PATH = /home/steve/dspicc/pic30/debs/usr/bin:/bin:/usr/kde/3.5/bin

CC = pic30-elf-gcc
AS = pic30-elf-as
STRIP = pic30-elf-strip

CFLAGS = -mcpu=30F4011 -Wunused-function -mpa=5 -I/home/steve/dspicc/c30/support/h -I/home/steve/dspicc/c30/include -I/home/steve/dspicc/c30/support/inc -Wall -fno-short-double

ASLAGS = -I/home/steve/dspicc/c30/include

LDFLAGS = -Wl,-L/home/steve/dspicc/c30/lib,--script=/home/steve/dspicc/c30/support/gld/p30f4011.gld,--heap=0,-lp30F4011-elf,--report-mem

BIN2HEX = pic30-elf-bin2hex

LIBS = /home/steve/dspicc/c30/lib/libp30F4011-elf.a

all: master.hex slave.hex

slave.hex: slave.o
	$(CC) $(LDFLAGS) $(LIBS) slave.o -o slave.elf
	$(BIN2HEX) slave.elf

master.hex: master.o
	$(CC) $(LDFLAGS) $(LIBS) master.o -o master.elf
	$(BIN2HEX) master.elf

.c.o: config.h
	$(CC) $(CFLAGS) -Wunused-function -c $< -o $@

run:
	piklab-prog -p icd2 -d 30f4011 -c run


master: master.hex
	piklab-prog -p icd2 -d 30f4011 -c program master.hex

slave: slave.hex
	piklab-prog -p icd2 -d 30f4011 -c program slave.hex




clean:
	rm -f *.o *.elf *.hex *~

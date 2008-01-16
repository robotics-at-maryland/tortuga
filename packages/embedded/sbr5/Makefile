PATH = /home/steve/dspicc/pic30/debs/usr/bin:/bin:/usr/kde/3.5/bin

CC = pic30-elf-gcc
AS = pic30-elf-as
STRIP = pic30-elf-strip

CFLAGS = -mcpu=30F4011 -Wunused-function -mpa=5 -I/home/steve/dspicc/c30/support/h -I/home/steve/dspicc/c30/include -I/home/steve/dspicc/c30/support/inc -Wall -fno-short-double

ASLAGS = -I/home/steve/dspicc/c30/include

LDFLAGS = -Wl,-L/home/steve/dspicc/c30/lib,--script=/home/steve/dspicc/c30/support/gld/p30f4011.gld,--heap=0,-lp30F4011-elf,--report-mem

BIN2HEX = pic30-elf-bin2hex

LIBS = /home/steve/dspicc/c30/lib/libp30F4011-elf.a

all: ic1.hex ic2.hex ic3.hex ic4.hex power.hex

power.o : power.c buscodes.h uart.c i2c.c
	$(CC) $(CFLAGS) -mcpu=30F5011 -Wunused-function -c $< -o $@

%.hex : %.elf
	$(BIN2HEX) $<

%.elf : %.o uart.c i2c.c
	$(CC) $(LDFLAGS) $(LIBS) $< -o $@

%.o : %.c buscodes.h uart.c i2c.c
	$(CC) $(CFLAGS) -Wunused-function -c $< -o $@

run:
	piklab-prog -p icd2 -d 30f4011 -c run

stop:
	piklab-prog -p icd2 -d 30f4011 -c stop

ic1: ic1.hex
	piklab-prog -p icd2 -d 30f4011 -c program ic1.hex

ic2: ic2.hex
	piklab-prog -p icd2 -d 30f4011 -c program ic2.hex

ic3: ic3.hex
	piklab-prog -p icd2 -d 30f4011 -c program ic3.hex

ic4: ic4.hex
	piklab-prog -p icd2 -d 30f4011 -c program ic4.hex

power: power.hex
	piklab-prog -p icd2 -d 30f4011 -c program power.hex

clean:
	rm -f *.o *.elf *.hex *~

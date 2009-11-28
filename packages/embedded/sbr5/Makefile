
TCPATH=/path/where/dspic/toolchain/got/untarred

PATH = $(TCPATH)/usr/bin:/bin:/usr/kde/3.5/bin

CC = pic30-elf-gcc
AS = pic30-elf-as
STRIP = pic30-elf-strip

CFLAGS = -Wunused-function -mpa=5 -I$(TCPATH)/c30/support/h -I$(TCPATH)/c30/include -I$(TCPATH)/c30/support/inc -Wall -fno-short-double

ASLAGS = -I$(TCPATH)/c30/include

LDFLAGS = -Wl,-L$(TCPATH)/c30/lib,--script=$(TCPATH)/c30/support/gld/p30f4011.gld,--heap=0,-lp30F4011-elf,--report-mem

BIN2HEX = pic30-elf-bin2hex

LIBS = $(TCPATH)/c30/lib/libp30F4011-elf.a


all: ic1.hex ic2.hex ic3.hex ic4.hex distro.hex balancer.hex bfin.hex debug.hex motor.hex

distro.o : distro.c buscodes.h uart.c i2c.c
	$(CC) -mcpu=30F6014A $(CFLAGS) -Wunused-function -c $< -o $@

balancer.o : balancer.c buscodes.h uart.c i2c.c
	$(CC) -mcpu=30F6014A $(CFLAGS) -Wunused-function -c $< -o $@

motor.o : motor.c buscodes.h typedef.h servo.h servo.c
	$(CC) -mcpu=30F6010A $(CFLAGS) -Wunused-function -c $< -o $@

motor.dumber.o : motor.dumber.c buscodes.h typedef.h servo.h servo.c
	$(CC) -mcpu=30F6010A $(CFLAGS) -Wunused-function -c $< -o $@

ic4.o : ic4.c buscodes.h uart.c i2c.c
	$(CC) -mcpu=30F4013 $(CFLAGS) -Wunused-function -c $< -o $@

distro.elf : distro.o uart.c i2c.c
	$(CC) -Wl,-L$(TCPATH)/c30/lib,--script=$(TCPATH)/c30/support/gld/p30f6014A.gld,--heap=0,-lp30F6014A-elf,--report-mem $(TCPATH)/c30/lib/libp30F6014A-elf.a $< -o $@

balancer.elf : balancer.o uart.c i2c.c
	$(CC) -Wl,-L$(TCPATH)/c30/lib,--script=$(TCPATH)/c30/support/gld/p30f6014A.gld,--heap=0,-lp30F6014A-elf,--report-mem $(TCPATH)/c30/lib/libp30F6014A-elf.a $< -o $@

motor.elf : motor.o
	$(CC) -Wl,-L$(TCPATH)/c30/lib,--script=$(TCPATH)/c30/support/gld/p30f6010A.gld,--heap=0,-lp30F6010A-elf,--report-mem $< $(TCPATH)/c30/lib/libp30F6010A-elf.a -o $@

motor.dumber.elf : motor.dumber.o
	$(CC) -Wl,-L$(TCPATH)/c30/lib,--script=$(TCPATH)/c30/support/gld/p30f6010A.gld,--heap=0,-lp30F6010A-elf,--report-mem $< $(TCPATH)/c30/lib/libp30F6010A-elf.a -o $@

ic4.elf : ic4.o uart.c i2c.c
	$(CC) -Wl,-L$(TCPATH)/c30/lib,--script=$(TCPATH)/c30/support/gld/p30f4013.gld,--heap=0,-lp30F4013-elf,--report-mem $(TCPATH)/c30/lib/libp30F4013-elf.a $< -o $@



%.hex : %.elf
	$(BIN2HEX) $<

%.elf : %.o uart.c i2c.c
	$(CC) $(LDFLAGS) $(LIBS) $< -o $@

%.o : %.c buscodes.h uart.c i2c.c
	$(CC) -mcpu=30F4011 $(CFLAGS) -Wunused-function -c $< -o $@

run:
	piklab-prog -p icd2 -d 30f4011 -c run

run2:
	piklab-prog -p icd2 -d 30f6014A -c run

stop:
	piklab-prog -p icd2 -d 30f4011 -c stop

ic1: ic1.hex
	piklab-prog -p icd2 -d 30f4011 -c program ic1.hex

ic2: ic2.hex
	piklab-prog -p icd2 -d 30f4011 -c program ic2.hex

ic3: ic3.hex
	piklab-prog -p icd2 -d 30f4011 -c program ic3.hex

ic4: ic4.hex
	piklab-prog -p icd2 -d 30f4013 -c program ic4.hex

bfin: bfin.hex
	piklab-prog -p icd2 -d 30f4011 -c program bfin.hex

distro: distro.hex
	piklab-prog -p icd2 -d 30f6014A -c program distro.hex

balancer: balancer.hex
	piklab-prog -p icd2 -d 30f6014A -c program balancer.hex

debug: debug.hex
	piklab-prog -p icd2 -d 30f4011 -c program debug.hex

clean:
	rm -f *.o *.elf *.hex *~

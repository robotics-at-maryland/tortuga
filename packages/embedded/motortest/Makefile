
TCPATH=/home/kickmyassman/robotics-toolchain/

PATH = $(TCPATH)/usr/bin:/bin:/usr/kde/3.5/bin

CC = pic30-elf-gcc
AS = pic30-elf-as
STRIP = pic30-elf-strip

CFLAGS = -Wunused-function -mpa=5 -I$(TCPATH)/c30/support/h -I$(TCPATH)/c30/include -I$(TCPATH)/c30/support/inc -Wall -fno-short-double

ASLAGS = -I$(TCPATH)/c30/include

LDFLAGS = -Wl,-L$(TCPATH)/c30/lib,--script=$(TCPATH)/c30/support/gld/p30f4011.gld,--heap=0,-lp30F4011-elf,--report-mem

BIN2HEX = pic30-elf-bin2hex

LIBS = $(TCPATH)/c30/lib/libp30F4011-elf.a


all: main.hex secondary.hex test_light_thingy.hex

%.hex : %.elf
	$(BIN2HEX) $<

%.elf : %.o
	$(CC) $(LDFLAGS) $(LIBS) $< -o $@

%.o : %.c
	$(CC) -mcpu=30F4011 $(CFLAGS) -Wunused-function -c $< -o $@

clean:
	rm -f *.o *.elf *.hex *~

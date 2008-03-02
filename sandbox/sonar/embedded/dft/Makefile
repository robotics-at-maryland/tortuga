DSPICC_PATH = /home/steve/dspicc

PATH = $(DSPICC_PATH)/pic30/debs/usr/bin:/bin

# Provide standard C headers that are missing from Linux-hosted PIC gcc toolchain
SURROGATE_STD_H_PATH = ../include

CC = pic30-elf-gcc
AS = pic30-elf-as
STRIP = pic30-elf-strip

CFLAGS = -mcpu=30F4011 -Wunused-function -mpa=5 -I$(DSPICC_PATH)/c30/support/h -I$(DSPICC_PATH)/c30/include -I$(DSPICC_PATH)/c30/support/inc -I$(SURROGATE_STD_H_PATH) -Wall -fno-short-double

ASLAGS = -I$(DSPICC_PATH)/c30/include

LDFLAGS = -Wl,-L$(DSPICC_PATH)/c30/lib,--script=$(DSPICC_PATH)/c30/support/gld/p30f4011.gld,--heap=0,-lp30F4011-elf,--report-mem

BIN2HEX = pic30-elf-bin2hex

LIBS = $(DSPICC_PATH)/c30/lib/libp30F4011-elf.a

all: main.hex

%.hex : %.elf
	$(BIN2HEX) $<

main.elf: main.o dft.o
	$(CC) $(LDFLAGS) $(LIBS) main.o dft.o -o main.elf


%.elf : %.o 
	$(CC) $(LDFLAGS) $(LIBS) $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	piklab-prog -p icd2 -d 30f4011 -c run

stop:
	piklab-prog -p icd2 -d 30f4011 -c stop

clean:
	rm -f *.o *.elf *.hex *~

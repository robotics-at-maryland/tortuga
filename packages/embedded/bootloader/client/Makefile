CC = gcc
CFLAGS = -Wall -g -finline -Werror

OBJS = dspphex.o dsppglob.o dsppmisc.o dsppmrow.o loader.o

all:	hcloader

clean:
	rm -f *.o *.obj *.map hcloader hcloader.exe

hcloader: $(OBJS) dspicprg.h serial.h linux.o
	gcc *.o -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
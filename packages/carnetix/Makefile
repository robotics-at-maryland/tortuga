
CC = gcc
LD = gcc
AR = ar

LDFLAGS= -lusb -lctx -L./bin -g
CFLAGS= -g -Iinclude

all: ctx consolemon

clean:
	rm -rf bin *~ ctx consolemon

# Building ctx library 
bin:
	mkdir -p bin

bin/ctxapi.o: src/ctxapi.c include/ctxapi.h bin
	$(CC) $(CFLAGS) -c src/ctxapi.c -o bin/ctxapi.o

bin/libctx.a: bin/ctxapi.o
	$(AR) rcs bin/libctx.a bin/ctxapi.o 

# Build Test/Demo Programs
test/bin:
	mkdir -p test/bin

# Test Program
bin/ctx.o: test/src/ctx.c include/ctxapi.h
	$(CC) $(CFLAGS) -c test/src/ctx.c -o bin/ctx.o 

ctx: bin/libctx.a bin/ctx.o
	$(CC) $(LDFLAGS)  bin/ctx.o bin/ctxapi.o -o ctx

# Demo Program
bin/consolemon.o: test/src/consolemon.c include/ctxapi.h
	$(CC) $(CFLAGS) -c test/src/consolemon.c -o bin/consolemon.o


consolemon: bin/libctx.a bin/consolemon.o
	$(CC) $(LDFLAGS) bin/consolemon.o bin/ctxapi.o -o consolemon 

$(CC)=gcc
LDFLAGS=lusb
CFLAGS=-g
all: ctx consolemon

clean:
	rm -f *.o *~ ctx consolemon

ctx.o: ctxapi.h
ctxapi.o: ctxapi.h

ctx: ctxapi.o ctx.o ctxapi.h
	gcc -lusb ctx.o ctxapi.o -g -o ctx

consolemon: ctxapi.o ctxapi.h consolemon.o
	gcc -lusb consolemon.o ctxapi.o -g -o consolemon 

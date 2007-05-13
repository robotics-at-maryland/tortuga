$(CC)=gcc
LDFLAGS=lusb

all: ctx

clean:
	rm -f *.o *~ ctx

ctx.o: ctxapi.h
ctxapi.o: ctxapi.h

ctx: ctxapi.o ctx.o ctxapi.h
	g++ -lusb ctx.o ctxapi.o -o ctx

$(CC)=gcc
LDFLAGS=lusb

all: ctx

clean:
	rm -f *.o *~ ctx

ctx: ctxapi.o ctx.o
	g++ -lusb ctx.o ctxapi.o -o ctx

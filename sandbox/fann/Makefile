
CHEADERS = 
CSOURCE = 
COBJECTS = ${CSOURCE:.c=.o}

CXXHEADERS = cvToFloat.h imageNetwork.h
CXXSOURCE = buildNetwork.cpp imageNetwork.cpp
CXXOBJECTS = ${CXXSOURCE:.cpp=.o}

INCDIR = /opt/ram/local/include
LIBDIR = /opt/ram/local/lib
LIBS = -lcv -lcxcore -lhighgui -lfann -lboost_filesystem-xgcc40-mt -lboost_system-xgcc40-mt -lstdc++

CC = gcc
CFLAGS = -O3 -Wall -pipe

all: buildNetwork

buildNetwork: $(COBJECTS) $(CXXOBJECTS)
	$(CC) $(CFLAGS) -L$(LIBDIR) $(LIBS) -o $@ $(COBJECTS) $(CXXOBJECTS)

$(COBJECTS): $(CSOURCE) $(CHEADERS)
	$(CC) $(CFLAGS) -I$(INCDIR) -x c -c -o $*.o $*.c

$(CXXOBJECTS): $(CXXSOURCE) $(CXXHEADERS) $(CHEADERS)
	$(CC) $(CFLAGS) -I$(INCDIR) -x c++ -c -o $*.o $*.cpp

clean:
	rm -f buildNetwork $(COBJECTS) $(CXXOBJECTS)
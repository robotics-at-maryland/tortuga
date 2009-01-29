
CHEADERS = 
CSOURCE = 
COBJECTS = ${CSOURCE:.c=.o}

CXXHEADERS = imageNetwork.hpp imageSupport.hpp
CXXSOURCE = imageNetwork.cpp imageSupport.cpp
CXXOBJECTS = ${CXXSOURCE:.cpp=.o}

EXESOURCE = buildNetwork.cpp testNetwork.cpp
EXEOBJECTS = ${EXESOURCE:.cpp=.o}
EXECUTABLES = ${EXEOBJECTS:.o=}

INCDIR = /opt/ram/local/include
LIBDIR = /opt/ram/local/lib
LIBS = -lcv -lcxcore -lhighgui -lfann -lboost_filesystem-xgcc40-mt -lboost_system-xgcc40-mt -lstdc++

CC = gcc
CFLAGS = -O3 -Wno-sign-compare -pipe

all: $(EXECUTABLES)

$(EXECUTABLES): $(COBJECTS) $(CXXOBJECTS) $(EXEOBJECTS)
	$(CC) $(CFLAGS) -L$(LIBDIR) $(LIBS) -o $@ $(COBJECTS) $(CXXOBJECTS) $@.o

$(COBJECTS): $(CSOURCE) $(CHEADERS)
	$(CC) $(CFLAGS) -I$(INCDIR) -x c -c -o $*.o $*.c

$(CXXOBJECTS) $(EXEOBJECTS): $(CXXSOURCE) $(CXXHEADERS) $(CHEADERS) $(EXESOURCE)
	$(CC) $(CFLAGS) -I$(INCDIR) -x c++ -c -o $*.o $*.cpp

clean:
	rm -f $(EXECUTABLES) $(EXEOBJECTS) $(COBJECTS) $(CXXOBJECTS)
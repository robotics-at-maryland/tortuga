
CHEADERS = 
CSOURCE = 
COBJECTS = ${CSOURCE:.c=.o}

CXXHEADERS = imageRecognizer.hpp
CXXSOURCE = imageRecognizer.cpp
CXXOBJECTS = ${CXXSOURCE:.cpp=.o}

EXESOURCE = network.cpp
EXEOBJECTS = ${EXESOURCE:.cpp=.o}
EXECUTABLES = ${EXEOBJECTS:.o=}

INCDIR = -I/opt/ram/local/include/boost-1_37 -I/opt/ram/local/include
LIBDIR = -L/opt/ram/local/lib
LIBS = -lcv -lcxcore -lhighgui -lfann -lboost_filesystem-xgcc40-mt -lboost_system-xgcc40-mt -lstdc++

CC = gcc
GENERAL = -pipe -Wno-sign-compare
NODEBUG = -O3 $(GENERAL)
DEBUG = -ggdb2 $(GENERAL)
CFLAGS = $(NODEBUG)

# default - just go wild!
all: $(EXECUTABLES)

# specific executable build rules
network: network.o imageRecognizer.o
	$(CC) $(CFLAGS) $(LIBDIR) $(LIBS) -o $@ $^

# general executable build rules - may link to uneeded objects (No longer needed)
#$(EXECUTABLES): $(EXEOBJECTS) $(CXXOBJECTS) $(COBJECTS)
#	$(CC) $(CFLAGS) $(LIBDIR) $(LIBS) -o $@ $(COBJECTS) $(CXXOBJECTS) $@.o

# how to build c objects
$(COBJECTS): $(CSOURCE) $(CHEADERS)
	$(CC) $(CFLAGS) $(INCDIR) -x c -c -o $*.o $*.c

# how to build c++ objects
$(CXXOBJECTS): $(CXXSOURCE) $(CXXHEADERS) $(CHEADERS) $(EXESOURCE)
	$(CC) $(CFLAGS) $(INCDIR) -x c++ -c -o $*.o $*.cpp

# how to build executable (main) cpp objects
# identical to the CXXOBJECTS rule except that the CXXSOURCE is not a dependancy
$(EXEOBJECTS): $(CXXHEADERS) $(CHEADERS) $(EXESOURCE)
	$(CC) $(CFLAGS) $(INCDIR) -x c++ -c -o $*.o $*.cpp

# get rid of everything so we can start over
clean:
	rm -f $(EXECUTABLES) $(EXEOBJECTS) $(COBJECTS) $(CXXOBJECTS)
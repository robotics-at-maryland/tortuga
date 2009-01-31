
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
GENERAL = -pipe -Wno-sign-compare
NODEBUG = -O3 $(GENERAL)
DEBUG = -ggdb2 $(GENERAL)
CFLAGS = $(NODEBUG)

# default - just go wild!
all: $(EXECUTABLES)

# specific executable build rules
buildNetwork: imageNetwork.o imageSupport.o buildNetwork.o
	$(CC) $(CFLAGS) -L$(LIBDIR) $(LIBS) -o buildNetwork $^
	
testNetwork: testNetwork.o imageNetwork.o 
	$(CC) $(CFLAGS) -L$(LIBDIR) $(LIBS) -o testNetwork $^

# general executable build rules - may link to uneeded objects (No longer needed)
#$(EXECUTABLES): $(EXEOBJECTS) $(CXXOBJECTS) $(COBJECTS)
#	$(CC) $(CFLAGS) -L$(LIBDIR) $(LIBS) -o $@ $(COBJECTS) $(CXXOBJECTS) $@.o

# how to build c objects
$(COBJECTS): $(CSOURCE) $(CHEADERS)
	$(CC) $(CFLAGS) -I$(INCDIR) -x c -c -o $*.o $*.c

# how to build c++ objects
$(CXXOBJECTS): $(CXXSOURCE) $(CXXHEADERS) $(CHEADERS) $(EXESOURCE)
	$(CC) $(CFLAGS) -I$(INCDIR) -x c++ -c -o $*.o $*.cpp

# how to build executable (main) cpp objects
# identical to the CXXOBJECTS rule except that the CXXSOURCE is not a dependancy
$(EXEOBJECTS): $(CXXHEADERS) $(CHEADERS) $(EXESOURCE)
	$(CC) $(CFLAGS) -I$(INCDIR) -x c++ -c -o $*.o $*.cpp

# get rid of everything so we can start over
clean:
	rm -f $(EXECUTABLES) $(EXEOBJECTS) $(COBJECTS) $(CXXOBJECTS)
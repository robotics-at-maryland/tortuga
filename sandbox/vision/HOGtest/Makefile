# Compile and link flags
CXX          =  g++
CFLAGS       = -Wall -g

# Compilation (add flags as needed)
CXXFLAGS    += `pkg-config opencv --cflags`

# Linking (add flags as needed)
LDFLAGS     += `pkg-config opencv --libs`


all: HOGtest.x

HOGtest.x: HOGtest.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) -o HOGtest.x HOGtest.cpp

clean:
	rm -r *.x *~ a.out
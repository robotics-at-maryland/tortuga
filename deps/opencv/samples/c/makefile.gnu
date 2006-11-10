CXX=g++
CXXFLAGS = -I"..\..\cxcore\include" -I"..\..\cv\include" \
    -I"..\..\cvaux\include" -I"..\..\ml\include" -I"..\..\otherlibs\highgui"
LINKFLAGS = -L"..\..\lib" -lcxcore -lcv -lcvaux -lml -lhighgui

SAMPLES_C = $(notdir $(patsubst %.c, %.exe, $(wildcard *.c)))
SAMPLES_CPP = $(notdir $(patsubst %.cpp, %.exe, $(wildcard *.cpp)))

SAMPLES = $(SAMPLES_C) $(SAMPLES_CPP)

all: $(SAMPLES)

%.exe: %.c
	@echo $@
	@$(CXX) $(CXXFLAGS) -o $@ $< $(LINKFLAGS)

%.exe: %.cpp
	@echo $@
	@$(CXX) $(CXXFLAGS) -o $@ $< $(LINKFLAGS)

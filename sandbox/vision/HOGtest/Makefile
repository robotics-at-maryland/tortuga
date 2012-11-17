# Compile and link flags
CXX          =  g++
CFLAGS       = -Wall -g

# Compilation (add flags as needed)
#CXXFLAGS    += `pkg-config opencv --cflags`

# Linking (add flags as needed)
#LDFLAGS     += `pkg-config opencv --libs`


all: HOGtest.x

HOGtest.x: HOGtest.cpp
	$(CXX) $(CFLAGS) -I/opt/ram/local/include -L/opt/ram/local/lib -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy -Wl,-rpath=/opt/ram/local/lib -lboost_filesystem -lboost_program_options  -lboost_system -o HOGtest.x HOGtest.cpp

clean:
	rm -r *.x *~ a.out
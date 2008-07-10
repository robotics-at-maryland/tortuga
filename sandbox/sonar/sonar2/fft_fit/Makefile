#-----Macros---------------------------------

BASEDIR = /home/levashom/auv/sonar/tdoa_estimator/fft_fit

INCLUDES = -I $(BASEDIR) -I $(BASEDIR)/include

# set up compiler and options
CXX = g++
CXXFLAGS = -g $(INCLUDES)

LIBS = -lgsl -lgslcblas

#       #-----File Dependencies----------------------

SRC = sonar_test.cpp src/fft_sonar.cpp src/sonar_quadfit.cpp src/time_diff.cpp

# targets
sonar_test: include/fft_sonar.h include/sonar_quadfit.h include/time_diff.h sonar_test.cpp src/fft_sonar.cpp src/sonar_quadfit.cpp src/time_diff.cpp
	$(CXX) -o sonar_test $(CXXFLAGS) $(LIBS) $(SRC)

clean:
	rm -rf sonar_test

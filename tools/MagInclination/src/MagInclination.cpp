#include <iostream>

// Project Includes
#include "imu/include/imuapi.h"
#include "math/include/Helpers.h"
using namespace ram::math;


/*

!!!the robot must remain stationary for this program to work!!!

this program is designed to estimate the local inclination of Earth's 
magnetic field.  It gathers a few moments worth of data points from
the IMU and filters them to obtain an accurate estimate of the local 
magnetic (m) and gravitational (g) vectors.  The angle between the 
magnetic and gravitational vectors yield can be obtained using the 
definition of a cross product.  The local inclination can finally be 
computed as 90 degrees minus the above calculated angle between m and g.

written by Joseph Gland  2007-07-07
*/

int main (){
  //warn user to not move sub
  std::cout << "Magnetic Inclination Finder" << std::endl;
  std::cout << "\n!!! Don't move the robot !!!\n" << std::endl;

  //create IMU object
  int fd = openIMU("/dev/imu");
  if(fd == -1){
    printf("Could not find the IMU\n");
    exit(1);
  }
  RawIMUData imuData;

  //set number of data points to collect
  int numPoints = 2000;
  //create storage arrays
  double mag[3][numPoints];
  double accel[3][numPoints];
  
  //collect the data
  for(int index=1; index<numPoints; index++){
    
  }
  
  return 0;
}


// STD Includes
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <unistd.h>

// Project Includes
#include "drivers/imu/include/imuapi.h"
#include "math/include/Vector3.h"
#include "core/include/AveragingFilter.h"

using namespace ram::math;
using namespace ram::core;

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

static const int POINT_COUNT = 2000;

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

  //create storage arrays
  AveragingFilter<Vector3, POINT_COUNT> mag(Vector3::ZERO);
  AveragingFilter<Vector3, POINT_COUNT> accel(Vector3::ZERO);

  //collect the data
  for(int index = 0; index < POINT_COUNT; index++){
    //read IMU
    readIMUData(fd, &imuData);
    //save to arrays
    
    mag.addValue(Vector3(imuData.magX, imuData.magY, imuData.magZ));
    accel.addValue(Vector3(imuData.accelX, imuData.accelY, imuData.accelZ));

    //sleep for 20 ms
    usleep(20000);
  }

  //average the data
  Vector3 averagedMag(mag.getValue());
  Vector3 averagedAccel(accel.getValue());

  //normalize mag and accel vectors
  averagedMag.normalise();
  averagedAccel.normalise();

  //now use  |m x g| = |m||g|sin(theta)
  Vector3 crossed(averagedMag.crossProduct(averagedAccel));
  
  double magnitudeM = averagedMag.length();
  double magnitudeG = averagedAccel.length();
  double magnitudeCross = crossed.length();
  double theta = asin((magnitudeCross)/(magnitudeM*magnitudeG))*180/M_PI;

  //now go from horizontal plane
  theta = 90 - theta;
  
  //print output to user
  std::cout << "The local magnetic inclination was calculated to be ";
  std::cout << theta << " degrees." << std::endl;
  
  //print mag and accel input
  std::cout << "Average mag (IMU Frame) = " << averagedMag << std::endl;
  std::cout << "Average accel (IMU Frame) = " << averagedAccel << std::endl;
  return 0;
}


// STD Includes
#include <iostream>
#include <cmath>
#include <unistd.h>

// Project Includes
#include "imu/include/imuapi.h"
#include "math/include/Helpers.h"
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

double findRowAverageOf3xN(double * pData, int row, int columns);

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
  AveragingFilter<double, POINT_COUNT> magX;
  AveragingFilter<double, POINT_COUNT> magY;
  AveragingFilter<double, POINT_COUNT> magZ;
  AveragingFilter<double, POINT_COUNT> accelX;
  AveragingFilter<double, POINT_COUNT> accelY;
  AveragingFilter<double, POINT_COUNT> accelZ;

  //collect the data
  for(int index=0; index < numPoints-1; index++){
    //read IMU
    readIMUData(fd, &imuData);
    //save to arrays
    
    magX.addValue(imuData.magX);
    magY.addValue(imuData.magY);
    magZ.addValue(imuData.magZ);

    accelX.addValue(imuData.accelX);
    accelY.addValue(imuData.accelY);
    accelZ.addValue(imuData.accelZ);

    //sleep for 20 ms
    usleep(20000);
  }

  //average the data
  double averagedMag[3];
  double averagedAccel[3];
  averagedMag[0] = magX.getValue();
  averagedMag[1] = magY.getValue();
  averagedMag[2] = magZ.getValue();
  averagedAccel[0] = accelX.getValue();
  averagedAccel[1] = accelY.getValue();
  averagedAccel[2] = accelZ.getValue();

  //normalize mag and accel vectors
  normalize3x1(averagedMag);
  normalize3x1(averagedAccel);

  //now use  |m x g| = |m||g|sin(theta)
  double crossed[3];
  crossProduct3x1by3x1(averagedMag, averagedAccel, crossed);
  double magnitudeM;
  magnitudeM = magnitude3x1(averagedMag);
  double magnitudeG;
  magnitudeG = magnitude3x1(averagedAccel);
  double magnitudeCross;
  magnitudeCross = magnitude3x1(crossed);
  double theta;
  theta = asin((magnitudeCross)/(magnitudeM*magnitudeG))*180/M_PI;

  //now go from horizontal plane
  theta = 90 - theta;
  
  //print output to user
  std::cout << "The local magnetic inclination was calculated to be ";
  std::cout << theta << " degrees." << std::endl;
  
  //print mag and accel input
  std::cout << "Average mag (IMU Frame) = " << averagedMag[0] << " " << averagedMag[1];
  std::cout << " " << averagedMag[2] << std::endl;
  std::cout << "Average accel (IMU Frame) = " << averagedAccel[0] << " " << averagedAccel[1];
  std::cout << " " << averagedAccel[2] << std::endl;
  return 0;
}


/*

*/
double findRowAverageOf3xN(double * pData, int row, int columns){
  double sum;
  double* rowPtr = pData + row;
  
  for(int i = 0; i < columns; i++){
    sum = sum + *(rowPtr+i); 
  }

  return sum/columns;
}

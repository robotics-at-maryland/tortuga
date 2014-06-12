
// STD Includes
#include <iostream>
#include <cmath>
#include <unistd.h>

// Project Includes
#include "vehicle/include/device/IMU.h"
#include "core/include/ConfigNode.h"
#include "math/include/Helpers.h"
#include "drivers/imu/include/imuapi.h"

using namespace ram::math;
using namespace ram::core;
using namespace ram::vehicle::device;  

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


double findRowAverageOf3xN(double * pData, int row, int columns);

int main (){
  //warn user to not move sub
  std::cout << "Magnetic Inclination Finder" << std::endl;
  std::cout << "\n!!! Don't move the robot !!!\n" << std::endl;
  
  // Create IMU Device
  IMU imu(ConfigNode::fromString("{}"));

  // Start IMU running in the background
  imu.background(5); 
  
  FilteredIMUData imuData;

  //set number of data points to collect
  int numPoints = 2000;
  //create storage arrays
  double mag[3][numPoints];
  double accel[3][numPoints];
  
  //collect the data
  for(int index=0; index < numPoints-1; index++){
    //read IMU
    imu.getFilteredState(imuData);
    //save to arrays
    mag[0][index] = imuData.magX;
    mag[1][index] = imuData.magY;
    mag[2][index] = imuData.magZ;
    accel[0][index] = imuData.accelX;
    accel[1][index] = imuData.accelY;
    accel[2][index] = imuData.accelZ;
    //sleep for 20 ms
    usleep(20000);
  }
  
  //average the data
  double averagedMag[3];
  double averagedAccel[3];
  averagedMag[0] = findRowAverageOf3xN(&mag[0][0],0,numPoints);
  averagedMag[1] = findRowAverageOf3xN(&mag[0][0],1,numPoints);
  averagedMag[2] = findRowAverageOf3xN(&mag[0][0],2,numPoints);
  averagedAccel[0] = findRowAverageOf3xN(&accel[0][0],0,numPoints);
  averagedAccel[1] = findRowAverageOf3xN(&accel[0][0],1,numPoints);
  averagedAccel[2] = findRowAverageOf3xN(&accel[0][0],2,numPoints);

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

  
  return 0;
}


/*

*/
double findRowAverageOf3xN(double * pData, int row, int columns){
  double sum = 0;
  double* rowPtr = pData + row;
  
  for(int i = 0; i < columns; i++){
    sum = sum + *(rowPtr+i);
  }

  return sum/columns;
}

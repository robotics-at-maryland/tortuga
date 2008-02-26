// STD Includes
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>

// UNIX Includes
#include <signal.h>
#include <unistd.h>

// Project Includes
#include "vehicle/include/device/IMU.h"
#include "core/include/ConfigNode.h"
#include "math/include/Vector3.h"
#include "imu/include/imuapi.h" // for FilteredIMUData

using namespace ram::math;
using namespace ram::core;
using namespace ram::vehicle::device;

double minX = 10;
double minY = 10;
double minZ = 10;
double maxX = -10;
double maxY = -10;
double maxZ = -10;

// Called on Ctrl+C does all the work
void sighandler(int signal)
{
    Vector3 min(minX, minY, minZ);
    Vector3 max(maxX, maxY, maxZ);
    std::cout << "Max " << max << std::endl;
    std::cout << "Min " << min << std::endl;
    Vector3 sum = (max + min) / 2;
    std::cout << "Bias " << sum << std::endl;
    std::cout << "magXBias: " << sum.x << std::endl;
    std::cout << "magYBias: " << sum.y << std::endl;
    std::cout << "magZBias: " << sum.z << std::endl;
    exit(0);
}

int main (){
  //warn user to not move sub
  std::cout << "Magnetic Bias Finder" << std::endl;
  std::cout << "\n!!! Rotate the around all 3 axes fully !!!\n" << std::endl;

  // Install signal handler
  if (SIG_ERR == signal(SIGINT, sighandler)) {
      std::cout << "Error install signal handler" << std::endl;
      return 2;
  }
  
  // Create IMU Device
  IMU imu(ConfigNode::fromString("{'name' : 'IMU',"
                                 "'magXBias' : 0.0,"
                                 "'magYBias' : 0.0,"
                                 "'magZBias' : 0.0}"));

  // Start IMU running in the background
  imu.background(5);

  std::cout << "array = [0 0 0;" << std::endl;
  
  FilteredIMUData imuData;
  for (int i = 0; i < 48000; i++)
//  while(true)
  {
      // Read IMU
      imu.getFilteredState(imuData);

      // max
      if (imuData.magX > maxX)
          maxX = imuData.magX;
      if (imuData.magY > maxY)
          maxY = imuData.magY;
      if (imuData.magZ > maxZ)
          maxZ = imuData.magZ;

      // max
      if (imuData.magX < minX)
          minX = imuData.magX;
      if (imuData.magY < minY)
          minY = imuData.magY;
      if (imuData.magZ < minZ)
          minZ = imuData.magZ;

      std::cout << imuData.magX << " " << imuData.magY << " "
                << imuData.magZ << ";" << std::endl;
      
      usleep(100000);
  }

  std::cout << "];"<< std::endl;
  
  
  return 0;
}

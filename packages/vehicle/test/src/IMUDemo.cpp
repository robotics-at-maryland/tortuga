/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/test/src/IMUDemo.cpp
 */

// STD Includes
#include <cmath>
#include <iostream>

// UNIX Includes
#include <unistd.h>

// Project Includes
#include "vehicle/include/device/IMU.h"
#include "core/include/ConfigNode.h"
#include "imu/include/imuapi.h"

using namespace ram::core;
using namespace ram::vehicle::device;

static const std::string BIASED_CFG = "{ 'magXBias' : -0.21305,"
                                      "'magYBias' : -0.139574,"
                                      "'magZBias' : -0.282475}";

static const std::string EMPTY_CFG = "{}";

int main()
{
    // Create IMU Device
    IMU imu(0, ConfigNode::fromString(BIASED_CFG));


    FilteredIMUData filtData;
    RawIMUData rawData;
    // Start IMU running in the background
    imu.background(5);

    while(1)
    {
      imu.getRawState(rawData);
      imu.getFilteredState(filtData);
	
      printf("IMU F. No Bias, Raw:: %7.4f %7.4f %7.4f\n", // Rot. & Filt. Mag: %7.4f %7.4f %7.4f\n",
	     rawData.magX, rawData.magY, rawData.magZ);
	     //	     filtData.magX, filtData.magY, filtData.magZ);
	//      std::cout << "Raw Mag " << imuData.magX << " " << imuData.magY 
	//		<< " " << imuData.magZ;

	//      std::c

	///	double magnitude = imuData.magX*imuData.magX+imuData.magY*imuData.magY
	//	  + imuData.magZ*imuData.magZ;
	//	magnitude = sqrt(magnitude);

	//	std::cout << " " << magnitude << std::endl;
	
	usleep(500 * 1000);
    }
    return 0;
}

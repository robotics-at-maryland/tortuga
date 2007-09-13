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
#include "core/include/AveragingFilter.h"
#include "imu/include/imuapi.h"
#include "math/include/Quaternion.h"

using namespace ram::math;
using namespace ram::core;
using namespace ram::vehicle::device;

static const std::string BIASED_CFG = "{ 'magXBias' : -0.21305,"
                                      "'magYBias' : -0.139574,"
                                      "'magZBias' : -0.282475,"
                                      "'localMagneticPitch' : 60}";

static const std::string EMPTY_CFG = "{}";

static const int POINT_COUNT = 2000;
static const int MS_SLEEP_TIME = 20;

int main()
{
    // Create IMU Device
    IMU imu(ConfigNode::fromString(BIASED_CFG));


    AveragingFilter<double, POINT_COUNT> quat1;
    AveragingFilter<double, POINT_COUNT> quat2;
    AveragingFilter<double, POINT_COUNT> quat3;
    AveragingFilter<double, POINT_COUNT> quat4;


    FilteredIMUData filtData;
    RawIMUData rawData;
    // Start IMU running in the background
    imu.background(5);

    for (int i = 0; i < POINT_COUNT; ++i)
    {
        imu.getRawState(rawData);
        imu.getFilteredState(filtData);
	Quaternion orientation(imu.getOrientation());

	quat1.addValue(orientation.w);
	quat2.addValue(orientation.x);
	quat3.addValue(orientation.y);
	quat4.addValue(orientation.z);

      /*      printf("IMU F. No Bias, Raw:: %7.4f %7.4f %7.4f\n", // Rot. & Filt. Mag: %7.4f %7.4f %7.4f\n",
	     rawData.magX, rawData.magY, rawData.magZ);
	     //	     filtData.magX, filtData.magY, filtData.magZ);*/
	//      std::cout << "Raw Mag " << imuData.magX << " " << imuData.magY 
	//		<< " " << imuData.magZ;

	//      std::c

	///	double magnitude = imuData.magX*imuData.magX+imuData.magY*imuData.magY
	//	  + imuData.magZ*imuData.magZ;
	//	magnitude = sqrt(magnitude);

	//	std::cout << " " << magnitude << std::endl;

	/*&         printf("%7.4f %7.4f %7.4f %7.4f;\n", orientation.q1,
		orientation.q2, orientation.q3,
		orientation.q4);*/

	
	usleep(MS_SLEEP_TIME * 1000);
    }


    std::cout << "Averaged Quat: [" << quat1.getValue() << ", " 
	      << quat2.getValue() << ", " 
	      << quat3.getValue() << ", " 
	      << quat4.getValue() << "]" << std::endl;

    return 0;
}

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
#include <csignal>

// UNIX Includes
#include <unistd.h>

// Project Includes
#include "vehicle/include/device/IMU.h"

#include "core/include/ConfigNode.h"
#include "core/include/AveragingFilter.h"

#include "math/include/Quaternion.h"

#include "drivers/imu/include/imuapi.h"

using namespace ram::math;
using namespace ram::core;
using namespace ram::vehicle::device;

/*static const std::string BIASED_CFG = "{ 'name' : 'IMU',"
                                      "'magXBias' : 0,"
                                      "'magYBias' : 0,"
                                      "'magZBias' : 0,"
                                      "'localMagneticPitch' : 60}";*/
static const std::string BIASED_CFG =
    "{ 'name' : 'IMU',"
    " 'magXBias' : -0.3053,"
    "'magYBias' : 0.2663,"
    "'magZBias' : 0.1884,"
    "'gyroXBias' : 0,"
    "'gyroYBias' : 0,"
    "'gyroZBias' : 0,"
    "'localMagneticPitch' : 60}";

static const std::string EMPTY_CFG = "{}";

static const int POINT_COUNT = 2000;
static const int MS_SLEEP_TIME = 20;
static bool RUNNING = true;

void sigIntHandler(int signal)
{
    RUNNING = false;
}

int main()
{

    // Setup signal handler
    if (SIG_ERR == signal(SIGINT, sigIntHandler)) {
	std::cout << "Error setting up signal handler. Aborted." << std::endl;
	exit(1);
    }

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

	quat1.addValue(orientation.x);
	quat2.addValue(orientation.y);
	quat3.addValue(orientation.z);
	quat4.addValue(orientation.w);

        std::cout << orientation.x << " " << orientation.y << " " << orientation.z
                  << " " << orientation.w << std::endl;
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
	if (!RUNNING) {
	    break;
	}
    }


    std::cout << "Averaged Quat: [" << quat1.getValue() << ", " 
	      << quat2.getValue() << ", " 
	      << quat3.getValue() << ", " 
	      << quat4.getValue() << "]" << std::endl;

    return 0;
}

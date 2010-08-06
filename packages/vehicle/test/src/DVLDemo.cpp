/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vehicle/test/src/DVLDemo.cpp
 */

// STD Includes
#include <iostream>
#include <csignal>

// UNIX Includes
#include <unistd.h>

// Project Includes
#include "vehicle/include/device/DVL.h"

#include "core/include/ConfigNode.h"
#include "core/include/AveragingFilter.h"

#include "math/include/Vector2.h"

#include "drivers/dvl/include/dvlapi.h"

using namespace ram::math;
using namespace ram::core;
using namespace ram::vehicle::device;

static const std::string EMPTY_CFG = "{ 'name' : 'DVL' }";

static const int POINT_COUNT = 2000;
static const int MS_SLEEP_TIME = 20;
static bool RUNNING = true;

void sigIntHandler(int signal) {
    RUNNING = false;
}

int main() {
    // Assign signal handler
    if (SIG_ERR == signal(SIGINT, sigIntHandler)) {
	std::cout << "Error setting SIGINT signal" << std::endl;
	exit(1);
    }

    DVL dvl(ConfigNode::fromString(EMPTY_CFG));

    AveragingFilter<double, POINT_COUNT> vect1;
    AveragingFilter<double, POINT_COUNT> vect2;

    RawDVLData rawData;
    dvl.background(5);

    for (int i = 0; i < POINT_COUNT; ++i) {
	dvl.getRawState(rawData);
	Vector2 vector2(dvl.getVelocity());

	std::cout << vector2.x << " " << vector2.y << " "
		  << rawData.valid << " "
		  << rawData.bt_velocity[0] << " "
		  << rawData.bt_velocity[1] << " "
		  << rawData.bt_velocity[2] << " "
		  << rawData.bt_velocity[3] << std::endl;

	if (!RUNNING)
	    break;

	usleep(MS_SLEEP_TIME * 1000);
    }

    std::cout << "Averaged Vector2: [" << vect1.getValue() << ", "
	      << vect2.getValue() << "]" << std::endl;

    return 0;
}

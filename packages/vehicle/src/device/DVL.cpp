/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vehicle/src/device/IMU.cpp
 */

/*
 * This is a work in progress. It shouldn't be used yet.
 */

// STD Includes
#include <iostream>
#include <cstdio>

// UNIX Includes
#include <unistd.h>  // for open()

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/device/DVL.h"

//static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("DVL"));

namespace ram {
namespace vehicle {
namespace device {

using namespace ram::math;
    
DVL::DVL(core::ConfigNode config, core::EventHubPtr eventHub,
         IVehiclePtr vehicle) :
    IVelocitySensor(eventHub, config["name"].asString()),
    Device(config["name"].asString()),
    Updatable(),
    m_devfile(config["devfile"].asString("/dev/dvl")),
    m_serialFD(-1),
    m_dvlNum(config["num"].asInt(0))
{
    // Need an api before I can do this
    //m_serialFD = openDVL(m_devfile.c_str());

    //LOGGER.info("% IMU#(0=main,1=boom) Accel Mag Gyro Accel-Raw Mag-Raw"
    //           " Gyro-Raw Quat TimeStamp");

    for (int i = 0; i < 5; ++i)
        update(1/50.0);
}

DVL::~DVL()
{
    // Always make sure to shutdwon the background thread
    Updatable::unbackground(true);

    // Only close file if its a non-negative number
    if (m_serialFD >= 0)
        close(m_serialFD);
}


void DVL::update(double timestep)
{
//    std::cout << "IMU update" << std::endl;
    // Only grab data on valid fd
    if (m_serialFD >= 0)
    {
	// Stuff goes here
    }
    // We didn't connect, try to reconnect
    else
    {
//        m_serialFD = openIMU(m_devfile.c_str());
    }
}
    
math::Vector3 DVL::getPosition()
{
    return m_position;
}

math::Vector2 DVL::getVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_velocityMutex);
    return m_velocity;
}
   
} // namespace device
} // namespace vehicle
} // namespace ram

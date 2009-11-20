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

#include "math/include/Helpers.h"
#include "math/include/Vector2.h"
#include "math/include/Events.h"

//static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("DVL"));

//#include "drivers/dvl/include/dvlapi.h"

namespace ram {
namespace vehicle {
namespace device {

using namespace ram::math;
    
DVL::DVL(core::ConfigNode config, core::EventHubPtr eventHub,
         IVehiclePtr vehicle) :
    IDVL(eventHub, config["name"].asString()),
    Device(config["name"].asString()),
    Updatable(),
    m_devfile(config["devfile"].asString("/dev/dvl")),
    m_serialFD(-1),
    m_dvlNum(config["num"].asInt(0)),
    m_depth(0),
    m_velocity(0, 0),
    m_rawState(0),
    m_filteredState(0)
{
    // Need an api before I can do this
    //m_serialFD = openDVL(m_devfile.c_str());

    //LOGGER.info("% DVL#(0=main,1=boom) Accel Mag Gyro Accel-Raw Mag-Raw"
    //           " Gyro-Raw Quat TimeStamp");

    for (int i = 0; i < 5; ++i)
        update(1/50.0);
}

DVL::~DVL()
{
    // Always make sure to shutdown the background thread
    Updatable::unbackground(true);

    // Only close file if its a non-negative number
    if (m_serialFD >= 0)
        close(m_serialFD);

    delete m_rawState;
    delete m_filteredState;
}


void DVL::update(double timestep)
{
    // Only grab data on valid fd
    if (m_serialFD >= 0)
    {
	// Stuff goes here
    }
    // We didn't connect, try to reconnect
    else
    {
//        m_serialFD = openDVL(m_devfile.c_str());
    }
}

double DVL::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_depthMutex);
    return m_depth;
}

math::Vector2 DVL::getVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_velocityMutex);
    return m_velocity;
}
   
} // namespace device
} // namespace vehicle
} // namespace ram

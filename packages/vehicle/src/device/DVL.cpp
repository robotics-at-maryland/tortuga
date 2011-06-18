/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vehicle/src/device/IMU.cpp
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
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/Events.h"


#include "math/include/Helpers.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Events.h"

#include "drivers/dvl/include/dvlapi.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("DVL"));

namespace ram {
namespace vehicle {
namespace device {

const int DVL::BAD_VELOCITY = -32768;

using namespace ram::math;
    
DVL::DVL(core::ConfigNode config, core::EventHubPtr eventHub,
         IVehiclePtr vehicle) :
    IVelocitySensor(eventHub, config["name"].asString()),
    Device(config["name"].asString()),
    Updatable(),
    m_vehicle(vehicle),
    m_devfile(config["devfile"].asString("/dev/dvl")),
    m_serialFD(-1),
    m_dvlNum(config["num"].asInt(0)),
    m_location(0, 0, 0),
    m_angOffset(0),
    m_bRt(math::Matrix2::IDENTITY),
    m_rawState(0)
{
    m_rawState = new RawDVLData();

    // construct rotation matrix for m_angOffset
    m_angOffset = config["angularOffset"].asDouble(0);
    double r_cos = cos(m_angOffset), r_sin = sin(m_angOffset);
    m_bRt = math::Matrix2(r_cos, -r_sin, r_sin, r_cos);

    // Need an api before I can do this
    m_serialFD = openDVL(m_devfile.c_str());

    if (m_serialFD >= 0)
        LOGGER.info("DVL has been successfully instantiated"
                    " with serial FD of %d", m_serialFD);
    else
        LOGGER.info("Could not connect with DVL");

    LOGGER.info("Velocity[0] Velocity[1]");
}

DVL::~DVL()
{
    // Always make sure to shutdown the background thread
    Updatable::unbackground(true);

    // Only close file if its a non-negative number
    if (m_serialFD >= 0)
        close(m_serialFD);

    delete m_rawState;
}


void DVL::update(double timestep)
{
    // Only grab data on valid fd
    if (m_serialFD >= 0)
    {
        RawDVLData newState;
        if (readDVLData(m_serialFD, &newState))
        {

            {
                // Thread safe copy of good dvl data
                core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
                *m_rawState = newState;
            }

            int xVel = newState.xvel_btm;
            int yVel = newState.yvel_btm;

            if(xVel != BAD_VELOCITY && yVel != BAD_VELOCITY)
            {
                // velocity in the body frame
                math::Vector2 velocity(newState.yvel_btm, newState.xvel_btm);
            
                RawDVLDataEventPtr event = RawDVLDataEventPtr(
                    new RawDVLDataEvent());
            
                event->name = getName();
                event->rawDVLData = newState;
                event->velocity_b = velocity;
                event->angularOffset = m_angOffset;
                event->timestep = timestep;

                publish(IVelocitySensor::RAW_UPDATE, event);

                LOGGER.infoStream() << velocity[0] << " "
                                    << velocity[1];
            }
        }
    }
    // We didn't connect, try to reconnect
    else
    {
        //m_serialFD = openDVL(m_devfile.c_str());
    }
}

math::Vector3 DVL::getLocation()
{
    return m_location;
}

void DVL::getRawState(RawDVLData& dvlState)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    dvlState = *m_rawState;
}

} // namespace device
} // namespace vehicle
} // namespace ram

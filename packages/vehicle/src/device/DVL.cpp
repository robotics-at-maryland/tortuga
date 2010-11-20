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
#include "vehicle/include/IVehicle.h"

#include "math/include/Helpers.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Events.h"

#include "drivers/dvl/include/dvlapi.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("DVL"));

namespace ram {
namespace vehicle {
namespace device {

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
    m_velocity(0, 0),
    m_location(0, 0, 0),
    bRt(math::Matrix2::IDENTITY),
    m_rawState(0)
{
    m_rawState = new RawDVLData();

    double angOffset = config["angularOffset"].asDouble(0);
    double r_cos = cos(angOffset), r_sin = sin(angOffset);
    bRt = math::Matrix2(r_cos, r_sin, -r_sin, r_cos);

    // Need an api before I can do this
    m_serialFD = openDVL(m_devfile.c_str());

    if (m_serialFD >= 0)
        LOGGER.info("DVL has been successfully instantiated"
                    " with serial FD of %d", m_serialFD);
    else
        LOGGER.info("Could not connect with DVL");

    // TODO: Temporary values until I know what to put in here
    LOGGER.info("% DVL#(0=main) Valid BottomTrack0 BottomTrack1"
		" BottomTrack2 BottomTrack3 Velocity ensembleNum"
                " year month day hour min sec hundredth TimeStamp");

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

        math::Vector2 oldVelocity;
        {
            core::ReadWriteMutex::ScopedReadLock lock(m_velocityMutex);
            oldVelocity = m_velocity;
        }

        // rotation matrix from transducer frame to body frame
        math::Matrix2 bRt(math::Matrix2::IDENTITY);

        // heads 1 and 2 are opposite, heads 3 and 4 are opposite
        // head 1 --> bt_velocity[0]
        // head 2 --> bt_velocity[1]
        // head 3 --> bt_velocity[2]
        // head 4 --> bt_velocity[3]

        // average the opposite velocities to get an estimate in the transducer frame
        double vel_t1 = (newState.bt_velocity[0] + newState.bt_velocity[1]) / 2;
        double vel_t2 = (newState.bt_velocity[2] + newState.bt_velocity[3]) / 2;

        // velocity in transducer frame
        math::Vector2 vel_t(vel_t1, vel_t2);

        // velocity in body frame
        math::Vector2 vel_b = bRt*vel_t;

        double yaw = m_vehicle->getOrientation().getYaw().valueRadians();
        
        math::Vector2 vel_n = math::nRb(yaw)*vel_b;

        {
            core::ReadWriteMutex::ScopedWriteLock lock(m_velocityMutex);
            m_velocity = vel_n;
//            m_position += (vel_n + oldVelocity)/2 * timestep;
        }

            LOGGER.infoStream() << newState.valid << " "
                                << newState.bt_velocity[0] << " "
                                << newState.bt_velocity[1] << " "
                                << newState.bt_velocity[2] << " "
                                << newState.bt_velocity[3] << " "
                                << vel_t << " "
                                << vel_b << " "
                                << newState.ensemblenum << " "
                                << newState.year << " "
                                << newState.month << " "
                                << newState.day << " "
                                << newState.hour << " "
                                << newState.min << " "
                                << newState.sec << " "
                                << newState.hundredth;

	    // Now publish the new velocity
	    math::Vector2EventPtr vevent(new math::Vector2Event());
	    // TODO: Insert whatever the local variable for velocity is
	    vevent->vector2 = vel_n;
	    publish(IVelocitySensor::UPDATE, vevent);
	}
    }
    // We didn't connect, try to reconnect
    else
    {	
	//m_serialFD = openDVL(m_devfile.c_str());
    }
}

math::Vector2 DVL::getVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_velocityMutex);
    return m_velocity;
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

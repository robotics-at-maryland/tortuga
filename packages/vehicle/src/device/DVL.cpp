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
    m_devfile(config["devfile"].asString("/dev/dvl")),
    m_serialFD(-1),
    m_dvlNum(config["num"].asInt(0)),
    m_velocity(0, 0),
    m_location(0, 0, 0),
    m_rawState(0),
    m_filteredState(0)
{
    m_rawState = new RawDVLData();
    m_filteredState = new FilteredDVLData();

    // Need an api before I can do this
    m_serialFD = openDVL(m_devfile.c_str());

    // TODO: Temporary values until I know what to put in here
    LOGGER.info("% DVL#(0=main) Valid BottomTrack0 BottomTrack1"
		" BottomTrack2 BottomTrack3 Velocity TimeStamp");

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
	RawDVLData newState;
	if (readDVLData(m_serialFD, &newState))
	{
	    {
		// Thread safe copy of good dvl data
		core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
		*m_rawState = newState;
	    }

	    // Work is going to be done to get the
	    // new velocity from the raw state
	    math::Vector2 velocity(0, 0);

            LOGGER.infoStream() << newState.valid << " "
				<< newState.bt_velocity[0] << " "
				<< newState.bt_velocity[1] << " "
				<< newState.bt_velocity[2] << " "
				<< newState.bt_velocity[3];

	    // Now publish the new velocity
	    math::Vector2EventPtr vevent(new math::Vector2Event());
	    // TODO: Insert whatever the local variable for velocity is
	    vevent->vector2 = velocity;
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

void DVL::getFilteredState(RawDVLData& dvlState)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    dvlState = *m_filteredState;
}
   
} // namespace device
} // namespace vehicle
} // namespace ram

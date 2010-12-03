/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/math/src/Events.cpp
 */

// Project Includes
#include "core/include/Feature.h"
#include "vehicle/include/Events.h"

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS)

#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::vehicle::PowerSourceEvent>
RAM_VEHICLE_POWERSOURCE_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::TempSensorEvent>
RAM_VEHICLE_TEMPSENSOR_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::ThrusterEvent>
RAM_VEHICLE_THRUSTER_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::SonarEvent>
RAM_VEHICLE_SONAR_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::RawIMUDataEvent>
RAM_VEHICLE_RAWIMUDATA_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::RawDVLDataEvent>
RAM_VEHICLE_RAWDVLDATA_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::RawDepthSensorDataEvent>
RAM_VEHICLE_RAWDEPTHSENSORDATA_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::ThrustUpdateEvent>
RAM_VEHICLE_THRUST_UPDATE_EVENT;

#endif // RAM_WITH_WRAPPERS

namespace ram {
namespace vehicle {

core::EventPtr PowerSourceEvent::clone()
{
    PowerSourceEventPtr event = PowerSourceEventPtr(new PowerSourceEvent());
    copyInto(event);
    event->id = id;
    event->enabled = enabled;
    event->inUse = inUse;
    event->voltage = voltage;
    event->current = current;
    return event;
}
    
core::EventPtr TempSensorEvent::clone()
{
    TempSensorEventPtr event = TempSensorEventPtr(new TempSensorEvent());
    copyInto(event);
    event->id = id;
    event->temp = temp;
    return event;
}

core::EventPtr ThrusterEvent::clone()
{
    ThrusterEventPtr event = ThrusterEventPtr(new ThrusterEvent());
    copyInto(event);
    event->address = address;
    event->current = current;
    event->enabled = enabled;
    return event;
}

core::EventPtr SonarEvent::clone()
{
    SonarEventPtr event = SonarEventPtr(new SonarEvent());
    copyInto(event);
    event->direction = direction;
    event->range = range;
    event->pingTimeSec = pingTimeSec;
    event->pingTimeUSec = pingTimeUSec;
    event->pingCount = pingCount;
    event->pingerID = pingerID;
    return event;
}

core::EventPtr RawIMUDataEvent::clone()
{
    RawIMUDataEventPtr event = RawIMUDataEventPtr(new RawIMUDataEvent());
    copyInto(event);

    event->name = name;
    event->rawIMUData = rawIMUData;
    event->magIsCorrupt = magIsCorrupt; 
    event->timestep = timestep;
 
    return event;
}

core::EventPtr RawDVLDataEvent::clone()
{
    RawDVLDataEventPtr event = RawDVLDataEventPtr(new RawDVLDataEvent());
    copyInto(event);

    event->name = name;
    event->rawDVLData = rawDVLData;
    event->velocity_b = velocity_b;
    event->timestep = timestep;

    return event;
}

core::EventPtr RawDepthSensorDataEvent::clone()
{
    RawDepthSensorDataEventPtr event = RawDepthSensorDataEventPtr(
        new RawDepthSensorDataEvent());
    copyInto(event);

    event->name = name;
    event->rawDepth = rawDepth;
    event->sensorLocation = sensorLocation;
    event->timestep = timestep;

    return event;
}

core::EventPtr ThrustUpdateEvent::clone()
{
    ThrustUpdateEventPtr event = ThrustUpdateEventPtr(new ThrustUpdateEvent());
    copyInto(event);

    event->forces = forces;
    event->torques = torques;

    return event;
}


} // namespace vehicle
} // namespace ram

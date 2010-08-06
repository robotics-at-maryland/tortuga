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

static ram::core::SpecificEventConverter<ram::vehicle::DepthSensorInitEvent>
RAM_VEHICLE_DEPTHSENSORINIT_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::IMUInitEvent>
RAM_VEHICLE_IMUINIT_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::DVLInitEvent>
RAM_VEHICLE_DVLINIT_EVENT;

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
    event->timestep = timestep;

    return event;
}

core::EventPtr RawDVLDataEvent::clone()
{
    RawDVLDataEventPtr event = RawDVLDataEventPtr(new RawDVLDataEvent());
    copyInto(event);

    event->name = name;
    event->rawDVLData = rawDVLData;
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
    event->timestep = timestep;

    return event;
}

core::EventPtr IMUInitEvent::clone()
{
    IMUInitEventPtr event = IMUInitEventPtr(new IMUInitEvent());
    copyInto(event);

    event->name = name;
    event->IMUtoVehicleFrame = math::Matrix3(IMUtoVehicleFrame);
    event->magBias = math::Vector3(magBias);
    event->gyroBias = math::Vector3(gyroBias);
    event->magCorruptThreshold = magCorruptThreshold;
    event->magNominalLength = magNominalLength;

    return event;
}

core::EventPtr DVLInitEvent::clone()
{
    DVLInitEventPtr event = DVLInitEventPtr(new DVLInitEvent());
    copyInto(event);

    event->name = name;
    event->angularOffset = angularOffset;

    return event;
}

core::EventPtr DepthSensorInitEvent::clone()
{
    DepthSensorInitEventPtr event = DepthSensorInitEventPtr(
        new DepthSensorInitEvent());
    copyInto(event);

    event->name = name;
    event->location = location;
    event->depthCalibSlope = depthCalibSlope;
    event->depthCalibIntercept = depthCalibIntercept;

    return event;
}


} // namespace vehicle
} // namespace ram

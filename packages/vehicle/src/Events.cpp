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

    
} // namespace vehicle
} // namespace ram

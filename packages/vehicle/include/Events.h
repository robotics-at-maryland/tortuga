/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/include/Events.h
 */

#ifndef RAM_VEHICLE_EVENTS_06_14_2008
#define RAM_VEHICLE_EVENTS_06_14_2008

// Project Includes
#include "core/include/Event.h"

namespace ram {
namespace vehicle {

struct PowerSourceEvent : public core::Event
{
    int id;
    bool enabled;
//    bool inUse;
    float voltage;
    float current;
};

typedef boost::shared_ptr<PowerSourceEvent> PowerSourceEventPtr;

struct TempSensorEvent : public core::Event
{
    int id;
    /** Temperature in degrees C */
    int temp;
};

typedef boost::shared_ptr<TempSensorEvent> TempSensorEventPtr;
    
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_EVENTS_06_14_2008

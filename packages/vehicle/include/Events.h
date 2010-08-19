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
#include "math/include/Events.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"

#include "drivers/imu/include/imuapi.h"
#include "drivers/dvl/include/dvlapi.h"
#include "vehicle/include/device/Common.h"

namespace ram {
namespace vehicle {

struct PowerSourceEvent : public core::Event
{
    int id;
    bool enabled;
    bool inUse;
    float voltage;
    float current;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<PowerSourceEvent> PowerSourceEventPtr;

struct TempSensorEvent : public core::Event
{
    int id;
    /** Temperature in degrees C */
    int temp;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<TempSensorEvent> TempSensorEventPtr;

struct ThrusterEvent : public core::Event
{
    int address;
    /** Current in Amps */
    double current;
    /** Whether or not the thruster is accepts commands */
    bool enabled;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<ThrusterEvent> ThrusterEventPtr;

struct SonarEvent : public core::Event
{
    /** Unit vector from the vehicle to the pinger */
    math::Vector3 direction;

    /** NOT USED */
    double range;

    /** sec part of the timeval struct*/
    int pingTimeSec;

    /** usec part of the timeval struct */
    int pingTimeUSec;

    /** Incremented for every new ping heard */
    int pingCount;

    /** The pinger from which the ping was found */
    unsigned char pingerID;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<SonarEvent> SonarEventPtr;


struct RawIMUDataEvent : public core::Event
{
    std::string name;
    RawIMUData rawIMUData;
    bool magIsCorrupt;
    double timestep;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<RawIMUDataEvent> RawIMUDataEventPtr;

struct RawDVLDataEvent : public core::Event
{
    std::string name;
    RawDVLData rawDVLData;
    double angOffset;
    double timestep;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<RawDVLDataEvent> RawDVLDataEventPtr;

struct RawDepthSensorDataEvent : public core::Event
{
    std::string name;
    double rawDepth;
    math::Vector3 sensorLocation;
    double timestep;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<RawDepthSensorDataEvent> RawDepthSensorDataEventPtr;

struct ThrustUpdateEvent : public core::Event
{
    math::Vector3 forces;
    math::Vector3 torques;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<ThrustUpdateEvent> ThrustUpdateEventPtr;


    
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_EVENTS_06_14_2008

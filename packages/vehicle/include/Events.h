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
    math::Vector3 sensorLocation;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<RawIMUDataEvent> RawIMUDataEventPtr;

struct RawDVLDataEvent : public core::Event
{
    RawDVLData rawDVLData;
    math::Vector3 sensorLocation;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<RawDVLDataEvent> RawDVLDataEventPtr;

struct RawDepthSensorDataEvent : public core::Event
{
    double rawDepth;
    math::Vector3 sensorLocation;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<RawDepthSensorDataEvent> RawDepthSensorDataEventPtr;


/* The following events exist so that when a sensor is created, it can publish
   its calibration values that might be needed by other parts of the code.  These
   values arent passed in the update events for efficiency purposes since they will
   not change during runtime */

struct IMUInitEvent : public core::Event
{
    std::string name;
    
    // rotation matrix from imu frame to vehicle frame
    math::Matrix3 IMUtoVehicleFrame;

    // [magXBias, magYBias, magZBias]
    math::Vector3 magBias;

    // [gyroXBias, gyroYBias, gyroZBias]
    math::Vector3 gyroBias;

    double magCorruptThreshold;
    double magNominalLength;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<IMUInitEvent> IMUInitEventPtr;

struct DVLInitEvent : public core::Event
{
    // not sure what all needs to go here

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<DVLInitEvent> DVLInitEventPtr;

struct DepthSensorInitEvent : public core::Event
{
    // not sure if anything needs to go here

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<DepthSensorInitEvent> DepthSensorInitEventPtr;
    
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_EVENTS_06_14_2008

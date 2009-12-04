/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockPositionSensor.h
 */

#ifndef RAM_VEHICLE_MOCKPOSITIONSENSOR_06_24_2009
#define RAM_VEHICLE_MOCKPOSITIONSENSOR_06_24_2009

// Project Includes
#include "vehicle/include/device/IPositionSensor.h"
#include "vehicle/include/device/Device.h"
#include "vehicle/include/Events.h"

class MockPositionSensor : public ram::vehicle::device::IPositionSensor,
                           public ram::vehicle::device::Device
{
public:
    MockPositionSensor(ram::core::ConfigNode config,
                       ram::core::EventHubPtr eventHub,
                       ram::vehicle::IVehiclePtr vehicle) :
        ram::vehicle::device::IPositionSensor(eventHub,
                                              config["name"].asString()),
        Device(config["name"].asString()),
        position(config["position"][0].asDouble(0),
                 config["position"][1].asDouble(0))
    {
    }

    
    MockPositionSensor(std::string name) :
        Device(name),
        position(0.0, 0.0),
        location(ram::math::Vector3::ZERO)
    {}

    ram::math::Vector2 position;
    ram::math::Vector3 location;

    virtual ram::math::Vector2 getPosition() { return position; }
    virtual ram::math::Vector3 getLocation() { return location; }

    void publishUpdate(ram::math::Vector2 update)
    {
        position = update;
        ram::vehicle::PositionEventPtr nevent(
            new ram::vehicle::PositionEvent());
        nevent->vector2 = update;
        publish(ram::vehicle::device::IPositionSensor::UPDATE, nevent);
    }
    
    virtual std::string getName() {
        return ram::vehicle::device::Device::getName();
    }

    virtual void setPriority(ram::core::IUpdatable::Priority) {};
    virtual ram::core::IUpdatable::Priority getPriority() {
        return ram::core::IUpdatable::NORMAL_PRIORITY;
    };
    virtual void setAffinity(size_t) {};
    virtual int getAffinity() {
        return -1;
    };
    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

#endif // RAM_VEHICLE_MOCKPOSITIONSENSOR_06_24_2009

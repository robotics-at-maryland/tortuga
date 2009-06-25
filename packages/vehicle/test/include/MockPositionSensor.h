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

class MockPositionSensor : public ram::vehicle::device::IPositionSensor,
                           public ram::vehicle::device::Device
{
public:
    MockPositionSensor(std::string name) :
        Device(name),
        position(0.0, 0.0),
        location(ram::math::Vector3::ZERO)
    {}

    ram::math::Vector2 position;
    ram::math::Vector3 location;

    virtual ram::math::Vector2 getPosition() { return position; }
    virtual ram::math::Vector3 getLocation() { return location; }
    
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

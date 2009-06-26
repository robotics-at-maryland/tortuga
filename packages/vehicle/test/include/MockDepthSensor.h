/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockDepthSensor.h
 */

#ifndef RAM_VEHICLE_MOCKDEPTHSENSOR_06_13_2008
#define RAM_VEHICLE_MOCKDEPTHSENSOR_06_13_2008

// Project Includes
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/Device.h"

class MockDepthSensor : public ram::vehicle::device::IDepthSensor,
                        public ram::vehicle::device::Device
{
public:
    MockDepthSensor(std::string name) :
        Device(name),
        depth(0.0),
        location(ram::math::Vector3::ZERO)
    {}

    double depth;
    ram::math::Vector3 location;

    virtual double getDepth() { return depth; }
    virtual ram::math::Vector3 getLocation() { return location; }

    void publishUpdate(double update)
    {
        depth = update;
        ram::math::NumericEventPtr nevent(new ram::math::NumericEvent());
        nevent->number = update;
        publish(ram::vehicle::device::IDepthSensor::UPDATE, nevent);
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

#endif // RAM_VEHICLE_MOCKDEPTHSENSOR_06_13_2008

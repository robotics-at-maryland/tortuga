/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockIMU.h
 */

#ifndef RAM_VEHICLE_MOCKIMU_10_29_2007
#define RAM_VEHICLE_MOCKIMU_10_29_2007

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IIMU.h"
#include "math/include/Events.h"

class MockIMU : public ram::vehicle::device::IIMU,
                public ram::vehicle::device::Device                
{
public:
    MockIMU(ram::core::ConfigNode config,
            ram::core::EventHubPtr eventHub,
            ram::vehicle::IVehiclePtr vehicle) :
        ram::vehicle::device::IIMU(eventHub, config["name"].asString()),
        Device(config["name"].asString())
    {
    }
    
    MockIMU(std::string name) : 
        IIMU(ram::core::EventHubPtr()), 
        Device(name)
	{}
	
    virtual ram::math::Vector3 getLinearAcceleration() 
    	{ return linearAcceleration; }

    virtual ram::math::Vector3 getMagnetometer()
    	{ return magnetometer; }
    
    virtual ram::math::Vector3 getAngularRate()
    	{ return angularRate; }
    
    ram::math::Vector3 linearAcceleration;
    ram::math::Vector3 magnetometer;
    ram::math::Vector3 angularRate;

    void publishUpdate(ram::math::Quaternion update)
    {
        ram::math::OrientationEventPtr orientationEvent(
            new ram::math::OrientationEvent());
        orientationEvent->orientation = update;
        publish(ram::vehicle::device::IIMU::UPDATE, orientationEvent);
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

#endif // RAM_VEHICLE_MOCKIMU_10_29_2007

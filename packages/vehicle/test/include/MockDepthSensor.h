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

// STD Includes
#include <cassert>

// Project Includes
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/Device.h"
#include "math/include/Events.h"

struct MockDepthPacket : public ram::vehicle::device::DepthPacket
{
    double updateDepth;

    MockDepthPacket(double depth) : updateDepth(depth) {}
};

class MockDepthSensor : public ram::vehicle::device::IDepthSensor,
                        public ram::vehicle::device::Device
{
public:
    MockDepthSensor(ram::core::ConfigNode config,
                    ram::core::EventHubPtr eventHub,
                    ram::vehicle::IVehiclePtr vehicle) :
        ram::vehicle::device::IDepthSensor(eventHub,config["name"].asString()),
        Device(config["name"].asString()),
        depth(config["depth"].asDouble(0)),
        location(ram::math::Vector3::ZERO)
    {
    }
    
    MockDepthSensor(std::string name) :
        Device(name),
        depth(0.0),
        location(ram::math::Vector3::ZERO)
    {}

    double depth;
    ram::math::Vector3 location;

    virtual double getDepth() { return depth;}
    virtual ram::math::Vector3 getLocation() { return location; }

    void publishUpdate(double update)
    {
	MockDepthPacket packet(update);
	assert(m_stateEstimator && "No state estimator assigned");
	m_stateEstimator->depthUpdate(&packet);

	// Publish the event
	ram::math::NumericEventPtr nevent(new ram::math::NumericEvent());
	nevent->number = depth;
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

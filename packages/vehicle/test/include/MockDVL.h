/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockDVL.h
 */

#ifndef RAM_VEHICLE_MOCKDVL_12_21_2009
#define RAM_VEHICLE_MOCKDVL_12_21_2009

// Project Includes
#include "vehicle/include/device/IDVL.h"
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/Device.h"

struct MockDVLPacket : public ram::vehicle::device::DVLPacket
{
    ram::math::Vector2 updateVel;

    MockDVLPacket(ram::math::Vector2 velocity) : updateVel(velocity) {}
};

class MockDVL : public ram::vehicle::device::IDVL,
                public ram::vehicle::device::Device
{
public:
    MockDVL(ram::core::ConfigNode config,
                       ram::core::EventHubPtr eventHub,
                       ram::vehicle::IVehiclePtr vehicle) :
        ram::vehicle::device::IDVL(eventHub,
				  config["name"].asString()),
        Device(config["name"].asString())
    {
	setVehicle(vehicle);
    }

    MockDVL(std::string name) :
        Device(name),
        location(ram::math::Vector3::ZERO)
    {}

    ram::math::Vector3 location;

    virtual ram::math::Vector3 getLocation() { return location; }

    void publishUpdate(ram::math::Vector2 update)
    {
	MockDVLPacket packet(update);
	assert(m_stateEstimator);
	m_stateEstimator->dvlUpdate(&packet);
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

#endif // RAM_VEHICLE_MOCKDVL_12_21_2009

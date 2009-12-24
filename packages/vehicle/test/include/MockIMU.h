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

// STD Includes
#include <cassert>

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/IIMU.h"
#include "math/include/Events.h"

struct MockIMUPacket : public ram::vehicle::device::IMUPacket
{
    ram::math::Quaternion updateQuat;

    MockIMUPacket(ram::math::Quaternion quaternion) : updateQuat(quaternion) {}
};

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

    void publishUpdate(ram::math::Quaternion update)
    {
	MockIMUPacket packet(update);
	assert(m_stateEstimator && "No state estimator assigned");
	m_stateEstimator->imuUpdate(&packet);

	// Publish the event
	ram::math::OrientationEventPtr oevent(
	    new ram::math::OrientationEvent());
	oevent->orientation = update;
	publish(ram::vehicle::device::IIMU::UPDATE, oevent);
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

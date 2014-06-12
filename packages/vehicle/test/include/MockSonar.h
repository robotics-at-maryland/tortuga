/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/MockSonar.h
 */

#ifndef RAM_VEHICLE_DEVICE_MOCKSONAR_06_27_2009
#define RAM_VEHICLE_DEVICE_MOCKSONAR_06_27_2009

// Project Includes
#include "vehicle/include/device/ISonar.h"
#include "vehicle/include/Events.h"

class RAM_EXPORT MockSonar : public ram::vehicle::device::ISonar,
                             public ram::vehicle::device::Device
             // boost::noncopyable
{
public:
    MockSonar(std::string name) :
        Device(name),
        direction(ram::math::Vector3::ZERO),
        pingTime(0)
    {}

    ram::math::Vector3 direction;
    ram::core::TimeVal pingTime;
    
    virtual ram::math::Vector3 getDirection() { return direction; }

    virtual double getRange() { return 0; }

    virtual ram::core::TimeVal getPingTime() { return pingTime; }

    void publishUpdate(ram::math::Vector3 direction_, unsigned char pingerID_)
    {
        // Make internal state match the event
        direction = direction_;

        // Send off the event
        ram::vehicle::SonarEventPtr sevent(new ram::vehicle::SonarEvent());
        sevent->direction = direction_;
        sevent->pingerID = pingerID_;
        publish(ram::vehicle::device::ISonar::UPDATE, sevent);
    }
    
    // IDevice Methods
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
    
#endif // RAM_VEHICLE_DEVICE_MOCKSONAR_06_27_2009

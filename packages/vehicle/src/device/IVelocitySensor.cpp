/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IVelocitySensor.cpp
 */

// Project Includes
#include "vehicle/include/device/IVelocitySensor.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IVelocitySensor, UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IVelocitySensor, RAW_UPDATE);

namespace ram {
namespace vehicle {
namespace device {

IVelocitySensor::IVelocitySensor(core::EventHubPtr eventHub,
                                 std::string name) :
    IDevice(eventHub, name)
{
}

IVelocitySensor::~IVelocitySensor()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

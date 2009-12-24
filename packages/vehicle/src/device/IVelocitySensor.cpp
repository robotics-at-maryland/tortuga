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

namespace ram {
namespace vehicle {
namespace device {

IVelocitySensor::IVelocitySensor(core::EventHubPtr eventHub,
                                 std::string name) :
    IStateEstimatorDevice(eventHub, name)
{
}

IVelocitySensor::~IVelocitySensor()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IDepthSensor.cpp
 */

// Project Includes
#include "vehicle/include/device/IDepthSensor.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IDepthSensor, UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IDepthSensor, RAW_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IDepthSensor, INIT);

namespace ram {
namespace vehicle {
namespace device {

IDepthSensor::IDepthSensor(core::EventHubPtr eventHub, std::string name) :
    IDevice(eventHub, name)
{
}

IDepthSensor::~IDepthSensor()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

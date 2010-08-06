/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/ITempSensor.cpp
 */

// Project Includes
#include "vehicle/include/device/ITempSensor.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::ITempSensor, UPDATE);

namespace ram {
namespace vehicle {
namespace device {

ITempSensor::ITempSensor(core::EventHubPtr eventHub, std::string name) :
    IDevice(eventHub, name)
{
}

ITempSensor::~ITempSensor()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IPositionSensor.cpp
 */

// Project Includes
#include "vehicle/include/device/IPositionSensor.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IPositionSensor, UPDATE);

namespace ram {
namespace vehicle {
namespace device {

IPositionSensor::IPositionSensor(core::EventHubPtr eventHub,
                                 std::string name) :
    IDevice(eventHub, name)
{
}

IPositionSensor::~IPositionSensor()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

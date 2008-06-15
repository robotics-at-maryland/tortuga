/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IVoltageSensor.cpp
 */

// Project Includes
#include "vehicle/include/device/IVoltageSensor.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IVoltageSensor, UPDATE);

namespace ram {
namespace vehicle {
namespace device {

IVoltageSensor::~IVoltageSensor()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

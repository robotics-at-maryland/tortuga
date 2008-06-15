/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/ICurrenSensor.cpp
 */

// Project Includes
#include "vehicle/include/device/ICurrentSensor.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::ICurrentSensor, UPDATE);

namespace ram {
namespace vehicle {
namespace device {

ICurrentSensor::~ICurrentSensor()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

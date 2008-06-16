/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IVoltageProvider.cpp
 */

// Project Includes
#include "vehicle/include/device/IVoltageProvider.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IVoltageProvider, UPDATE);

namespace ram {
namespace vehicle {
namespace device {

IVoltageProvider::~IVoltageProvider()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

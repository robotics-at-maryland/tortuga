/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/ICurrenProvider.cpp
 */

// Project Includes
#include "vehicle/include/device/ICurrentProvider.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::ICurrentProvider, UPDATE);

namespace ram {
namespace vehicle {
namespace device {

ICurrentProvider::~ICurrentProvider()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

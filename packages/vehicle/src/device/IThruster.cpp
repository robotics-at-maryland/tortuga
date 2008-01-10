/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IThruster.cpp
 */

// Project Includes
#include "vehicle/include/device/IThruster.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IThruster, FORCE_UPDATE);

namespace ram {
namespace vehicle {
namespace device {
    
IThruster::~IThruster()
{    
}

} // namespace device
} // namespace vehicle
} // namespace ram

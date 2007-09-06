/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/Thruster.cpp
 */

// STD Includes
#include <sstream>

// Project Includes
#include "vehicle/include/device/Device.h"

namespace ram {
namespace vehicle {
namespace device {

Device::Device(std::string name) :
    m_name(name)
{
}

std::string Device::getName()
{
    return m_name;
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

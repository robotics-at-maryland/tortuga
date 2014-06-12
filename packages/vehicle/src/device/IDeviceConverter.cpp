/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/vehicle/src/device/IDeviceConverter.cpp
 */

// Project Includes
#include "vehicle/include/device/IDeviceConverter.h"

namespace ram {
namespace core {

template<>
vehicle::device::IDeviceConverter::ConverterRegistry*
vehicle::device::IDeviceConverter::getConverterRegistry()
{
    static vehicle::device::IDeviceConverter::ConverterRegistry
        deviceConverterRegistry;
    return &deviceConverterRegistry;
}

} // namespace core
} // namespace ram

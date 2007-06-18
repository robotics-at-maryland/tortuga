/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Common.h
 */

#ifndef RAM_VEHICLE_COMMON_06_11_2006
#define RAM_VEHICLE_COMMON_06_11_2006

// STD Includes
#include <string>
#include <map>

// Project Includes
#include "vehicle/include/device/Common.h"

namespace ram {
namespace vehicle {

// Forward Declartions
class Vehicle;

// Typedefs
typedef std::map<std::string, device::Device*> NameDeviceMap;
typedef std::map<std::string, int> TemperatureMap;

} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_COMMON_06_11_2006

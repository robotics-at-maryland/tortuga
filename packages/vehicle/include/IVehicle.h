/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Vehicle.h
 */

#ifndef RAM_VEHICLE_VEHICLE_06_11_2006
#define RAM_VEHICLE_VEHICLE_06_11_2006

// Project Includes
#include "vehicle/include/Common.h"
#include "vehicle/include/IUpdatable.h"

namespace ram {
namespace vehicle {

class IVehicle : public IUpdatable
{
public:
    /** Gets the device associated with the given name */
    device::Device* getDevice(std::string name) = 0;

    /** Gets the list of current temperatures */
    TemperatureMap getTemperatures() = 0;
    
private:
    std::map<std::string, Device*> m_devices;
};

} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_VEHICLE_06_11_2006

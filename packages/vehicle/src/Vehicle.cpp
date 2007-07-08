/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Vehicle.cpp
 */

// STD Includes
#include <iostream>

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/device/IDevice.h"

namespace ram {
namespace vehicle {

Vehicle::Vehicle(core::ConfigNode config) :
    m_config(config)
{
}

device::IDevicePtr Vehicle::getDevice(std::string name)
{
    return m_devices[name];
}

double Vehicle::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    return m_state.depth;
}

void Vehicle::getState(Vehicle::VehicleState& state)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    state = m_state;
}

void Vehicle::setState(const Vehicle::VehicleState& state)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_state_mutex);
    m_state = state;
}

void Vehicle::_addDevice(device::IDevicePtr device)
{
    m_devices[device->getName()] = device;
}

void Vehicle::update(double timestep)
{
    std::cout << "Step: " << timestep << std::endl;
}
    
} // namespace vehicle
} // namespace ram

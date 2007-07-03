/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Vehicle.cpp
 */

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/device/IDevice.h"

namespace ram {
namespace vehicle {

Vehicle::Vehicle()
{
}

/*Vehicle::~Vehicle()
{
    m_devices.clear();
    }*/
    
device::IDevicePtr Vehicle::getDevice(std::string name)
{
    return m_devices[name];
}

math::Vector3 Vehicle::getLinearAcceleration()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    return m_state.linear_acceleration;
}

math::Vector3 Vehicle::getAngularRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    return m_state.angular_rate;
}

math::Quaternion Vehicle::getOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    return m_state.orientation;
}

double Vehicle::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    return m_state.depth;
}

double Vehicle::getVoltage()
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
    
} // namespace vehicle
} // namespace ram

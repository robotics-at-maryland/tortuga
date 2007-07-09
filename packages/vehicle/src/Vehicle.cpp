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
#include "sensorapi/include/sensorapi.h"

namespace ram {
namespace vehicle {

Vehicle::Vehicle(core::ConfigNode config) :
    m_config(config),
    m_sensorFD(-1)
{
    std::string devfile =
        m_config["sensor_board_file"].asString("/dev/sensor");

    m_sensorFD = openSensorBoard(devfile.c_str());
    syncBoard(m_sensorFD);

    if (m_sensorFD < -1)
        std::cout << "Could not open sensor board\n";
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

void Vehicle::safeThruster()
{
    if (m_sensorFD >= 0)
    {
        thrusterSafety(m_sensorFD, CMD_THRUSTER1_ON);
        thrusterSafety(m_sensorFD, CMD_THRUSTER2_ON);
        thrusterSafety(m_sensorFD, CMD_THRUSTER3_ON);
        thrusterSafety(m_sensorFD, CMD_THRUSTER4_ON);
    }
}

void Vehicle::unsafeThrusters()
{
    if (m_sensorFD >= 0)
    {
        thrusterSafety(m_sensorFD, CMD_THRUSTER1_OFF);
        thrusterSafety(m_sensorFD, CMD_THRUSTER2_OFF);
        thrusterSafety(m_sensorFD, CMD_THRUSTER3_OFF);
        thrusterSafety(m_sensorFD, CMD_THRUSTER4_OFF);
    }
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
    if (m_sensorFD >= 0)
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_state_mutex);
        m_state.depth = readDepth(m_sensorFD);
    }
}
    
} // namespace vehicle
} // namespace ram

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/IMU.cpp
 */

// UNIX Includes
#include <unistd.h>  // for open()

// Project Includes
#include "vehicle/include/device/IMU.h"

#include "imu/include/imuapi.h"
    
namespace ram {
namespace vehicle {
namespace device {

IMU::IMU(Vehicle* vehicle, core::ConfigNode config) :
    Device(vehicle, config["name"].asString()),
    Updatable(),
    Subject(),
    m_devfile(config["devfile"].asString()),
    m_serialFD(-1),
    m_rawState(0)
{
    m_serialFD = openIMU(m_devfile.c_str());
    m_rawState = new RawIMUData();
}

IMU::~IMU()
{
    // Always make sure to shutdwon the background thread
    Updatable::unbackground(true);

    // Only close file if its a non-negative number
    if (m_serialFD >= 0)
        close(m_serialFD);

    delete m_rawState;
}

IMUPtr IMU::construct(Vehicle* vehicle, core::ConfigNode config)
{
    return IMUPtr(new IMU(vehicle, config));
}
    
void IMU::update(double timestep)
{
    // Only grab data on valid fd
    if (m_serialFD >= 0)
    {
        // Grab latest state from vehicle
        RawIMUData newState;
        if (readIMUData(m_serialFD, &newState))
        {
            {
                // Thread safe copy of good imu data
                core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
                *m_rawState = newState;
            }

            // Nofity observers
            setChanged();
            notifyObservers(0, DataUpdate);
        }
        
    }
    // We didn't connect, try to reconnect
    else
    {
        m_serialFD = openIMU(m_devfile.c_str());
    }
}

void IMU::getRawState(RawIMUData& imuState)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    imuState = *m_rawState;
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

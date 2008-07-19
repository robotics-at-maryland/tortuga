/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/IMU.cpp
 */

// Library Includes
#include <boost/assign/list_of.hpp> // for 'list_of()'

// Project Includes
#include "vehicle/include/device/PSU.h"
#include "drivers/carnetix/include/ctxapi.h"

namespace ba = boost::assign;

namespace ram {
namespace vehicle {
namespace device {

PSU::PSU(core::ConfigNode config, core::EventHubPtr eventHub,
         IVehiclePtr vehicle) :
    IPSU(eventHub),
    Device(config["name"].asString()),
    Updatable(),
    m_config(config),
    m_ctxValues(0),
    m_deviceHandle(0)
{
    m_deviceHandle = ctxInit();
    m_ctxValues = new struct ctxValues();
}

PSU::~PSU()
{
    // Always make sure to shutdwon the background thread
    Updatable::unbackground(true);

    // Only close file if its a non-negative number
    if (m_deviceHandle)
        ctxClose(m_deviceHandle);

    delete m_ctxValues;
}

void PSU::update(double timestep)
{
    // Only grab data on valid fd
    if (m_deviceHandle)
    {
        // Grab latest state from vehicle
        struct ctxValues newCtxValues;
        if (-1 == ctxReadValues(m_deviceHandle,
                                &newCtxValues))
        {
            {
                // Thread safe copy of good imu data
                core::ReadWriteMutex::ScopedWriteLock lock(m_valuesMutex);
                *m_ctxValues = newCtxValues;
            }

            // Nofity observers
            //setChanged();
            //notifyObservers(0, DataUpdate);
        }
        
    }
    // We didn't connect, try to reconnect
    else
    {
        //m_serialFD = openIMU(m_devfile.c_str());
    }
}

double PSU::getBatteryVoltage()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_valuesMutex);
    return m_ctxValues->battVoltage;
}


double PSU::getBatteryCurrent()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_valuesMutex);
    return m_ctxValues->battCurrent;
}

double PSU::getTotalWattage()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_valuesMutex);
    return  m_ctxValues->battVoltage * m_ctxValues->battCurrent;
}
   

VoltageList PSU::getVoltages()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_valuesMutex);
    return ba::list_of<double>(m_ctxValues->priVoltage)
        (m_ctxValues->secVoltage);
}

CurrentList PSU::getCurrents()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_valuesMutex);
    return ba::list_of<double>(m_ctxValues->priCurrent)
        (m_ctxValues->secCurrent);
}

WattageList PSU::getWattages()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_valuesMutex);
    return ba::list_of<double>(m_ctxValues->priVoltage *
                               m_ctxValues->priCurrent)
        (m_ctxValues->secVoltage * m_ctxValues->secCurrent);
}   

StringList PSU::getSupplyNames()
{
    return ba::list_of<std::string>("Primary")("Secondary");
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

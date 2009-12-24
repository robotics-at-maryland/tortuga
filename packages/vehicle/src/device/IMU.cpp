/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/IMU.cpp
 */

// STD Includes
#include <iostream>
#include <cstdio>

// UNIX Includes
#include <unistd.h>  // for open()

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/device/IMU.h"
#include "vehicle/include/Common.h"
#include "vehicle/include/device/IStateEstimator.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"
#include "math/include/Events.h"

#include "drivers/imu/include/imuapi.h"

namespace ram {
namespace vehicle {
namespace device {

using namespace ram::math;
    
IMU::IMU(core::ConfigNode config, core::EventHubPtr eventHub,
         IVehiclePtr vehicle) :
    IIMU(eventHub, config["name"].asString()),
    Device(config["name"].asString()),
    Updatable(),
    m_devfile(config["devfile"].asString("/dev/imu")),
    m_serialFD(-1),
    m_imuNum(config["num"].asInt(0)),
    m_heartbeat(0),
    m_rawState(0)
{
    m_serialFD = openIMU(m_devfile.c_str());
    m_rawState = new RawIMUData();

    // Preload configuration data into the packet
    m_packet.IMUToVehicleFrame[0][0] =
        config["imuToVehicleRotMatrix"][0][0].asDouble(0);
    m_packet.IMUToVehicleFrame[0][1] =
        config["imuToVehicleRotMatrix"][0][1].asDouble(0);
    m_packet.IMUToVehicleFrame[0][2] =
        config["imuToVehicleRotMatrix"][0][2].asDouble(1);
    
    m_packet.IMUToVehicleFrame[1][0] =
        config["imuToVehicleRotMatrix"][1][0].asDouble(-1);
    m_packet.IMUToVehicleFrame[1][1] =
        config["imuToVehicleRotMatrix"][1][1].asDouble(0);
    m_packet.IMUToVehicleFrame[1][2] =
        config["imuToVehicleRotMatrix"][1][2].asDouble(0);
    
    m_packet.IMUToVehicleFrame[2][0] =
        config["imuToVehicleRotMatrix"][2][0].asDouble(0);
    m_packet.IMUToVehicleFrame[2][1] =
        config["imuToVehicleRotMatrix"][2][1].asDouble(-1);
    m_packet.IMUToVehicleFrame[2][2] =
        config["imuToVehicleRotMatrix"][2][2].asDouble(0);

        // Load Bias Values
    m_packet.magXBias = config["magXBias"].asDouble();
    m_packet.magYBias = config["magYBias"].asDouble();
    m_packet.magZBias = config["magZBias"].asDouble();
    m_packet.gyroXBias = config["gyroXBias"].asDouble();
    m_packet.gyroYBias = config["gyroYBias"].asDouble();
    m_packet.gyroZBias = config["gyroZBias"].asDouble();

    
    // Load Magnetic Corruption Threshold, default is ridiculously large acceptable range
    m_packet.magCorruptThresh = config["magCorruptThresh"].asDouble(2);

    // Load nominal value of magnetic vector length
    m_packet.magNominalLength = config["magNominalLength"].asDouble(0.24);


    for (int i = 0; i < 5; ++i)
        update(1/50.0);
}

IMU::~IMU()
{
    // Always make sure to shut down the background thread
    Updatable::unbackground(true);

    // Only close file if its a non-negative number
    if (m_serialFD >= 0)
        close(m_serialFD);

    delete m_rawState;
}


void IMU::update(double timestep)
{
//    std::cout << "IMU update" << std::endl;
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

		// Thread safe update of state estimator
		m_packet.imuNum = m_imuNum;
		m_packet.timestep = timestep;
		m_packet.rawData = &newState;
		m_packet.device = IIMUPtr(this);

		// Send the raw data to the state estimator
		m_stateEstimator->imuUpdate(&m_packet);

		// Publish the heartbeat
		math::NumericEventPtr hevent(new math::NumericEvent());
		hevent->number = ++m_heartbeat;
		publish(vehicle::device::IIMU::UPDATE, hevent);
	    }

        }
    }
    // We didn't connect, try to reconnect
    else
    {
//        m_serialFD = openIMU(m_devfile.c_str());
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

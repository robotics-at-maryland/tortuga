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
#include "vehicle/include/Events.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"
#include "math/include/Events.h"

#include "drivers/imu/include/imuapi.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("IMU"));

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
    m_magXBias(0),
    m_magYBias(0),
    m_magZBias(0),
    m_gyroXBias(0),
    m_gyroYBias(0),
    m_gyroZBias(0),
    m_magCorruptThresh(100),
    m_magNominalLength(0),
    m_rawState(0)
{
    m_serialFD = openIMU(m_devfile.c_str());
    m_rawState = new RawIMUData();

    // Load Rotation Matrix
    m_IMUToVehicleFrame[0][0] =
        config["imuToVehicleRotMatrix"][0][0].asDouble(0);
    m_IMUToVehicleFrame[0][1] =
        config["imuToVehicleRotMatrix"][0][1].asDouble(0);
    m_IMUToVehicleFrame[0][2] =
        config["imuToVehicleRotMatrix"][0][2].asDouble(1);
    
    m_IMUToVehicleFrame[1][0] =
        config["imuToVehicleRotMatrix"][1][0].asDouble(-1);
    m_IMUToVehicleFrame[1][1] =
        config["imuToVehicleRotMatrix"][1][1].asDouble(0);
    m_IMUToVehicleFrame[1][2] =
        config["imuToVehicleRotMatrix"][1][2].asDouble(0);
    
    m_IMUToVehicleFrame[2][0] =
        config["imuToVehicleRotMatrix"][2][0].asDouble(0);
    m_IMUToVehicleFrame[2][1] =
        config["imuToVehicleRotMatrix"][2][1].asDouble(-1);
    m_IMUToVehicleFrame[2][2] =
        config["imuToVehicleRotMatrix"][2][2].asDouble(0);

    // Load Bias Values
    m_magXBias = config["magXBias"].asDouble();
    m_magYBias = config["magYBias"].asDouble();
    m_magZBias = config["magZBias"].asDouble();
    m_gyroXBias = config["gyroXBias"].asDouble();
    m_gyroYBias = config["gyroYBias"].asDouble();
    m_gyroZBias = config["gyroZBias"].asDouble();

    
    // Load Magnetic Corruption Threshold, default is ridiculously large acceptable range
    m_magCorruptThresh = config["magCorruptThresh"].asDouble(2);

    // Load nominal value of magnetic vector length
    m_magNominalLength = config["magNominalLength"].asDouble(0.24);
     
    //    printf("Bias X: %7.5f Bias Y: %7.5f Bias Z: %7.5f\n", m_magXBias, 
    //        m_magYBias, m_magZBias);
    LOGGER.info("% IMU#(0=main,1=boom) Accel Mag Gyro TimeStamp");

    // what is the purpose of this?
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

            /* Take the raw data, put it into OGRE format applying the
             * bias corrections.
             */
            math::Vector3 linearAccel_debiased(newState.accelX,
                                               newState.accelY,
                                               newState.accelZ);

            math::Vector3 mag_debiased(newState.magX - m_magXBias,
                                       newState.magY - m_magYBias,
                                       newState.magZ - m_magZBias);

            math::Vector3 gyro_debiased(newState.gyroX - m_gyroXBias,
                                        newState.gyroY - m_gyroYBias,
                                        newState.gyroZ - m_gyroZBias);

            math::Matrix3 imuToVehicleFrame(m_IMUToVehicleFrame);

            /* Rotate the data from the IMU frame to the vehicle frame */
            math::Vector3 rotatedLinearAccel = 
                imuToVehicleFrame * linearAccel_debiased;

            math::Vector3 rotatedMagnetometer = 
                imuToVehicleFrame * mag_debiased;

            math::Vector3 rotatedGyro = 
                imuToVehicleFrame * gyro_debiased;


            RawIMUData rotatedState;
            rotatedState.accelX = rotatedLinearAccel[0];
            rotatedState.accelY = rotatedLinearAccel[1];
            rotatedState.accelZ = rotatedLinearAccel[2];

            rotatedState.magX = rotatedMagnetometer[0];
            rotatedState.magY = rotatedMagnetometer[1];
            rotatedState.magZ = rotatedMagnetometer[2];

            rotatedState.gyroX = rotatedGyro[0];
            rotatedState.gyroY = rotatedGyro[1];
            rotatedState.gyroZ = rotatedGyro[2];

            RawIMUDataEventPtr event = RawIMUDataEventPtr(
                new RawIMUDataEvent());
            event->name = getName();
            event->rawIMUData = rotatedState;
            if(corflg == false)
            {
                event->magIsCorrupt = false;
            }
            else
            {
                event->magIsCorrupt = true;
            }
            event->timestep = timestep;
            publish(IIMU::RAW_UPDATE, event);

            LOGGER.infoStream() << m_imuNum << " "
                                << rotatedState.accelX << " "
                                << rotatedState.accelY << " "
                                << rotatedState.accelZ << " "
                                << rotatedState.magX << " "
                                << rotatedState.magY << " "
                                << rotatedState.magZ << " "
                                << rotatedState.gyroX << " "
                                << rotatedState.gyroY << " "
                                << rotatedState.gyroZ;
        }
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

/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/modules/BasicIMUEstimationModule.cpp
 */

/* These algorithms were originally coded by Joseph Galante.  I tried to
 * copy them and make them a little more readable by removing unused parts.
 * They have now been moved to the estimation utilities.
 */

// STD Includes
#include <iostream>

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "math/include/Helpers.h"

#include "estimation/include/Utility.h"
#include "estimation/include/modules/BasicIMUEstimationModule.h"

#include "vehicle/include/device/IIMU.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstIMU"));

namespace ram {
namespace estimation {

BasicIMUEstimationModule::BasicIMUEstimationModule(core::ConfigNode config,
                                                   core::EventHubPtr eventHub,
                                                   EstimatedStatePtr estState) :
    EstimationModule(eventHub,"BasicIMUEstimationModule",estState,
                     vehicle::device::IIMU::RAW_UPDATE),
    m_magIMUName(config["magIMUNum"].asString("MagBoom")),
    m_cgIMUName(config["cgIMUNum"].asString("IMU")),
    m_gyroXThresh(config["xThresh"].asDouble(0)),
    m_gyroYThresh(config["xThresh"].asDouble(0)),
    m_gyroZThresh(config["xThresh"].asDouble(0))
{
    /* initialization of estimator from config values should be done here */
    LOGGER.info("% IMU-Name[1] Accel[3] Mag[3] Gyro[3] Accel-Raw[3] Mag-Raw[3]"
                " Gyro-Raw[3] Quat[4] TimeStamp[1]");
    lx = 0;
    ly = 0;
    lz = 0;
}

void BasicIMUEstimationModule::update(core::EventPtr event)
{
    vehicle::RawIMUDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawIMUDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        LOGGER.warn("BasicIMUEstimationModule: update: Invalid Event Type");
        return;
    }

    /* This is where the estimation should be done
       The result should be stored in m_estimatedState */

    std::string name = ievent->name;
   
    if(name != m_magIMUName && name != m_cgIMUName)
    {
        LOGGER.warn("BasicIMUEstimationModule: update: Invalid IMU Name");
        return;
    }

    double timestep = ievent->timestep;
    bool magIsCorrupt = ievent->magIsCorrupt;
    imuList.insert(name);

    if(m_filteredState.find(name) == m_filteredState.end())
    {
        FilteredIMUDataPtr filteredData = FilteredIMUDataPtr(
            new FilteredIMUData());
        m_filteredState[name] = filteredData;
    }

    /* grab the new state and filter it */
    RawIMUData newState = ievent->rawIMUData;

    m_filteredAccelX[name].addValue(newState.accelX); 
    m_filteredAccelY[name].addValue(newState.accelY);
    m_filteredAccelZ[name].addValue(newState.accelZ);
    
    m_filteredMagX[name].addValue(newState.magX);
    m_filteredMagY[name].addValue(newState.magY);
    m_filteredMagZ[name].addValue(newState.magZ);

    m_filteredGyroX[name].addValue(newState.gyroX);
    m_filteredGyroY[name].addValue(newState.gyroY);
    m_filteredGyroZ[name].addValue(newState.gyroZ);
    m_filteredGyroXV.addValue((newState.gyroX-lx)/timestep);
    m_filteredGyroYV.addValue((newState.gyroY-ly)/timestep);
    m_filteredGyroZV.addValue((newState.gyroZ-lz)/timestep);


    if(m_filteredState.find(name) == m_filteredState.end())
    {
        LOGGER.warn("IMU filtered state has not been created yet.");
        return;
    }

    FilteredIMUDataPtr filteredState = m_filteredState[name];

    if(!filteredState)
    {
        LOGGER.warn("IMU filtered state smart pointer is empty.");
        return;
    }

    filteredState->accelX = m_filteredAccelX[name].getValue();
    filteredState->accelY = m_filteredAccelY[name].getValue();
    filteredState->accelZ = m_filteredAccelZ[name].getValue();
 
    filteredState->magX = m_filteredMagX[name].getValue();
    filteredState->magY = m_filteredMagY[name].getValue();
    filteredState->magZ = m_filteredMagZ[name].getValue();
    
    filteredState->gyroX = m_filteredGyroX[name].getValue();
    filteredState->gyroY = m_filteredGyroY[name].getValue();
    filteredState->gyroZ = m_filteredGyroZ[name].getValue();

    /* Pull the averaged values from the averaging filter and put them
     * into OGRE format for the following calculations
     */

    if(m_filteredState.find(m_cgIMUName) == m_filteredState.end())
    {
        LOGGER.warn("CG IMU not yet created");
        return;
    }

    if(m_filteredState.find(m_magIMUName) == m_filteredState.end())
    {
        LOGGER.warn("MAGBOOM IMU not yet created");
        return;
    }

    if(!(m_filteredState[m_cgIMUName]))
    {
        LOGGER.warn("CG IMU is empty.");
        return;
    }

    if(!(m_filteredState[m_magIMUName]))
    {
        LOGGER.warn("MAGBOOM IMU is empty");
        return;
    }
        
    if(imuList.find(m_cgIMUName) == imuList.end())
    {
        LOGGER.warn("CG IMU not created yet so we cannot estimate anything");
        return;
    }

    math::Vector3 mag, accel;
    {
        core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);

        accel[0] = m_filteredState[m_cgIMUName]->accelX;
        accel[1] = m_filteredState[m_cgIMUName]->accelY;
        accel[2] = m_filteredState[m_cgIMUName]->accelZ;
        
        mag[0] = m_filteredState[m_cgIMUName]->magX;
        mag[1] = m_filteredState[m_cgIMUName]->magY;
        mag[2] = m_filteredState[m_cgIMUName]->magZ;
    }

    math::Quaternion estOrientation = math::Quaternion::IDENTITY;
    
    if(imuList.find(m_magIMUName) != imuList.end() && magIsCorrupt == false) 
    {

        /* If we have the magboom IMU, compute the quaternion from the
         * magnetometer reading from the magboom IMU and the linear
         * acceleration reading from the CG IMU
         */

        {
            core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);

            mag[0] = m_filteredState[m_magIMUName]->magX;
            mag[1] = m_filteredState[m_magIMUName]->magY;
            mag[2] = m_filteredState[m_magIMUName]->magZ;
        }
        // LOGGER.info("quatFromMagAccel - With MagBoom");
        estOrientation = estimation::Utility::quaternionFromMagAccel(mag,accel);

    } 
    else if (!magIsCorrupt)
    {

        /* If we dont have the magboom IMU and the magnetometer
         * reading is not corrupted, compute the estimated orientation
         * from the mag and accel readings from the single IMU
         */

        // LOGGER.info("quatFromMagAccel - Without MagBoom");
        estOrientation = estimation::Utility::quaternionFromMagAccel(mag,accel);

    } 
    else
    {

        /* If we dont have the magboom IMU and the magnetometer
         * reading is corrupted(or if the magnetometer is just corrupt),
         * compute the estimated orientation from
         * the previous quaternion and the angular rate
         */
        
        math::Vector3 omega(0,0,0);
        {
            core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
            if(abs(m_filteredGyroXV.getValue()) > m_gyroXThresh)
            {
                omega[0] = m_filteredState[m_cgIMUName]->gyroX;
            }
            if(abs(m_filteredGyroYV.getValue()) > m_gyroYThresh)
            {
                omega[1] = m_filteredState[m_cgIMUName]->gyroY;
            }
            if(abs(m_filteredGyroZV.getValue()) > m_gyroZThresh)
            {
                omega[2] = m_filteredState[m_cgIMUName]->gyroZ;
            }
        }

        math::Quaternion oldOrientation = 
            m_estimatedState->getEstimatedOrientation();

        // LOGGER.info("quatFromRate - No Boom, Mag Corrupted");
        estOrientation = estimation::Utility::quaternionFromRate(oldOrientation,
                                                                 omega,
                                                                 timestep);
    }
    /*
     * Updating only the z because x and y are updated in the kalman filter
    math::Vector3 estLinearAccel(m_filteredState[m_cgIMUName]->accelX,
                                 m_filteredState[m_cgIMUName]->accelY,
                                 m_filteredState[m_cgIMUName]->accelZ);
    */
    math::Vector3 prev(m_estimatedState->getEstimatedLinearAccel());
    math::Vector3 estLinearAccel(prev[0],
                                 prev[1],
                                 m_filteredState[m_cgIMUName]->accelZ);

    math::Vector3 estAngularRate(m_filteredState[m_cgIMUName]->gyroX,
                                 m_filteredState[m_cgIMUName]->gyroY,
                                 m_filteredState[m_cgIMUName]->gyroZ);

    // Update local storage of previous orientation and estimator
    m_estimatedState->setEstimatedOrientation(estOrientation);
    m_estimatedState->setEstimatedLinearAccel(estLinearAccel);
    m_estimatedState->setEstimatedAngularRate(estAngularRate);

    // Log data directly
    LOGGER.infoStream() << name << " "
                        << m_filteredState[name]->accelX << " "
                        << m_filteredState[name]->accelY << " "
                        << m_filteredState[name]->accelZ << " "
                        << m_filteredState[name]->magX << " "
                        << m_filteredState[name]->magY << " "
                        << m_filteredState[name]->magZ << " "
                        << m_filteredState[name]->gyroX << " "
                        << m_filteredState[name]->gyroY << " "
                        << m_filteredState[name]->gyroZ << " "
                        << newState.accelX << " "
                        << newState.accelY << " "
                        << newState.accelZ << " "
                        << newState.magX << " " 
                        << newState.magY << " "
                        << newState.magZ << " "
                        << newState.gyroX << " " 
                        << newState.gyroY << " "
                        << newState.gyroZ << " "
                        << estOrientation[0] << " " << estOrientation[1] << " "
                        << estOrientation[2] << " " << estOrientation[3];
}

} // namespace estimation
} // namespace ram

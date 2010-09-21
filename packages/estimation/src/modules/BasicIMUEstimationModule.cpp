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
 */

// STD Includes
#include <iostream>

// Library Includes
#include <boost/smart_cast.hpp>
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
                                                   core::EventHubPtr eventHub) :
    EstimationModule(eventHub,"BasicIMUEstimationModule"),
    m_magIMUName(config["magIMUNum"].asString("MagBoom")),
    m_cgIMUName(config["cgIMUNum"].asString("IMU"))
{
    /* initialization of estimator from config values should be done here */
    LOGGER.info("% IMU-Name[1] Accel[3] Mag[3] Gyro[3] Accel-Raw[3] Mag-Raw[3]"
                " Gyro-Raw[3] Quat[4] TimeStamp[1]");
}

void BasicIMUEstimationModule::update(core::EventPtr event,
                                      EstimatedStatePtr estimatedState)
{

    vehicle::RawIMUDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawIMUDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        LOGGER.warn("BasicIMUEstimationModule: update: Invalid Event Type");
        return;
    }

    /* This is where the estimation should be done
       The result should be stored in estimatedState */

    std::string name = ievent->name;
    double timestep = ievent->timestep;
    bool magIsCorrupt = ievent->magIsCorrupt;
    imuList.insert(name);

    RawIMUData newState = ievent->rawIMUData;
//    rotateAndFilterData(&newState, name);
 

    /* Pull the averaged values from the averaging filter and put them
     * into OGRE format for the following calculations
     */

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
    
    if(imuList.find(m_magIMUName) != imuList.end()) {

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

        estOrientation = estimation::Utility::quaternionFromMagAccel(mag,accel);

    } else if (magIsCorrupt) {

        /* If we dont have the magboom IMU and the magnetometer
         * reading is not corrupted, compute the estimated orientation
         * from the mag and accel readings from the single IMU
         */

        estOrientation = estimation::Utility::quaternionFromMagAccel(mag,accel);

    } else {

        /* If we dont have the magboom IMU and the magnetometer
         * reading is corrupted, compute the estimated orientation from
         * the previous quaternion and the angular rate
         */

        math::Vector3 omega;
        {
            core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);

            omega[0] = m_filteredState[m_cgIMUName]->gyroX;
            omega[1] = m_filteredState[m_cgIMUName]->gyroY;
            omega[2] = m_filteredState[m_cgIMUName]->gyroZ;
        }

        math::Quaternion oldOrientation = estimatedState->getEstimatedOrientation();

        estOrientation = estimation::Utility::quaternionFromRate(oldOrientation,
                                                              omega,
                                                              timestep);

    }

    // Update local storage of previous orientation and estimator
    estimatedState->setEstimatedOrientation(estOrientation);

    // Send Event
    math::OrientationEventPtr oevent(new math::OrientationEvent());
    oevent->orientation = estOrientation;
    publish(vehicle::device::IIMU::UPDATE, oevent);

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

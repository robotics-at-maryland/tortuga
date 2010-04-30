/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/BasicStateEstimator.cpp
 */

// Library Includes
#include <boost/bind.hpp>
#include <iostream>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IVelocitySensor.h"
#include "core/include/EventConnection.h"
#include "core/include/Event.h"
#include "vehicle/estimator/include/BasicStateEstimator.h"
#include "vehicle/include/Events.h"

namespace ram {
namespace estimator {

BasicStateEstimator::BasicStateEstimator(core::ConfigNode config, 
                                         core::EventHubPtr eventHub,
                                         vehicle::IVehiclePtr vehicle) :
    StateEstimatorBase(config,eventHub),
    m_vehicle(vehicle::IVehiclePtr(vehicle))
{
    // Connect the event listeners to their respective events
    if(eventHub != core::EventHubPtr()){
        updateConnection_IMU = eventHub->subscribeToType(
            vehicle::device::IIMU::RAW_UPDATE,
            boost::bind(&BasicStateEstimator::rawUpdate_IMU,this, _1));

        updateConnection_DepthSensor = eventHub->subscribeToType(
            vehicle::device::IDepthSensor::RAW_UPDATE,
            boost::bind(&BasicStateEstimator::rawUpdate_DepthSensor,this, _1));

        updateConnection_DVL = eventHub->subscribeToType(
            vehicle::device::IVelocitySensor::RAW_UPDATE,
            boost::bind(&BasicStateEstimator::rawUpdate_DVL,this, _1));

        //std::cout << "Event Listeners Registered." << std::endl;
    }
}

BasicStateEstimator::~BasicStateEstimator()
{
    if(updateConnection_IMU)
        updateConnection_IMU->disconnect();

    if(updateConnection_DVL)
        updateConnection_DVL->disconnect();

    if(updateConnection_DepthSensor)
        updateConnection_DepthSensor->disconnect();

    if(updateConnection_Sonar)
        updateConnection_Sonar->disconnect();
    
    if(updateConnection_Vision)
        updateConnection_Vision->disconnect();
}

void BasicStateEstimator::rawUpdate_DVL(core::EventPtr event)
{
    vehicle::RawDVLDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDVLDataEvent>(event);
    //std::cout << "rawUpdate_DVL_Basic" << std::endl;
}

void BasicStateEstimator::rawUpdate_IMU(core::EventPtr event)
{
    vehicle::RawIMUDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawIMUDataEvent>(event);

    math::Quaternion rawOrientation = m_vehicle->getRawOrientation();
    estimatedState->setEstimatedOrientation(rawOrientation);

    //std::cout << "rawUpdate_IMU_Basic: " << ievent->name << std::endl;
}

void BasicStateEstimator::rawUpdate_DepthSensor(core::EventPtr event)
{
    vehicle::RawDepthSensorDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDepthSensorDataEvent>(event);

    //std::cout << "rawUpdate_DepthSensor_Basic" << std::endl;
}

void BasicStateEstimator::update_Vision(core::EventPtr event)
{
    //std::cout << "rawUpdate_Vision_Basic" << std::endl;
}

void BasicStateEstimator::update_Sonar(core::EventPtr event)
{
    //std::cout << "rawUpdate_Sonar_Basic" << std::endl;
}

} // namespace estimator
} // namespace ram

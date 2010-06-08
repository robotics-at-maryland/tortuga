/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/ModularStateEstimator.cpp
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
#include "vehicle/estimator/include/ModularStateEstimator.h"
#include "vehicle/include/Events.h"

namespace ram {
namespace estimator {

ModularStateEstimator::ModularStateEstimator(core::ConfigNode config, 
                                             core::EventHubPtr eventHub,
                                             vehicle::IVehiclePtr vehicle) :
    StateEstimatorBase(config,eventHub),
    dvlEstimationModule(EstimationModulePtr()),
    imuEstimationModule(EstimationModulePtr()),
    depthEstimationModule(EstimationModulePtr()),
    rawDVLDataEvent(core::EventPtr()),
    rawIMUDataEvent(core::EventPtr()),
    rawBoomIMUDataEvent(core::EventPtr()),
    rawDepthDataEvent(core::EventPtr()),
    m_vehicle(vehicle::IVehiclePtr(vehicle))
{
    // Connect the event listeners to their respective events
    if(eventHub != core::EventHubPtr()){
        updateConnection_IMU = eventHub->subscribeToType(
            vehicle::device::IIMU::RAW_UPDATE,
            boost::bind(&ModularStateEstimator::rawUpdate_IMU,this, _1));

        updateConnection_DepthSensor = eventHub->subscribeToType(
            vehicle::device::IDepthSensor::RAW_UPDATE,
            boost::bind(&ModularStateEstimator::rawUpdate_DepthSensor,this, _1));

        updateConnection_DVL = eventHub->subscribeToType(
            vehicle::device::IVelocitySensor::RAW_UPDATE,
            boost::bind(&ModularStateEstimator::rawUpdate_DVL,this, _1));

        initConnection_IMU = eventHub->subscribeToType(
            vehicle::device::IIMU::INIT,
            boost::bind(&ModularStateEstimator::init_IMU,this,_1));

        initConnection_DVL = eventHub->subscribeToType(
            vehicle::device::IVelocitySensor::INIT,
            boost::bind(&ModularStateEstimator::init_DVL,this,_1));

        initConnection_DepthSensor = eventHub->subscribeToType(
            vehicle::device::IDepthSensor::INIT,
            boost::bind(&ModularStateEstimator::init_DepthSensor,this,_1));
    }

    // Construct the estimation modules
    dvlEstimationModule = EstimationModulePtr(
        new BasicDVLEstimationModule(config["DVLEstimationModule"]));
    imuEstimationModule = EstimationModulePtr(
        new BasicIMUEstimationModule(config["IMUEstimationModule"]));
    depthEstimationModule = EstimationModulePtr(
        new BasicDepthEstimationModule(config["DepthEstimationModule"]));
}



ModularStateEstimator::~ModularStateEstimator()
{
    /* unbind the update functions */
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

    /* unbind the init functions */
    if(initConnection_IMU)
        initConnection_IMU->disconnect();

    if(initConnection_DVL)
        initConnection_DVL->disconnect();
   
    if(initConnection_DepthSensor)
        initConnection_DepthSensor->disconnect();
}

void ModularStateEstimator::init_DVL(core::EventPtr event)
{
    dvlEstimationModule->init(event);
}

void ModularStateEstimator::init_IMU(core::EventPtr event)
{
    imuEstimationModule->init(event);
}

void ModularStateEstimator::init_DepthSensor(core::EventPtr event)
{
    depthEstimationModule->init(event);
}

void ModularStateEstimator::rawUpdate_DVL(core::EventPtr event)
{
    vehicle::RawDVLDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDVLDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        std::cerr << "ModularStateEstimator: rawUpdate_DVL: Invalid Event" 
                  << std::endl;
        return;
    }
        
    /* Update the estimated state by using an estimation module */
    dvlEstimationModule->update(ievent, estimatedState);
}

void ModularStateEstimator::rawUpdate_IMU(core::EventPtr event)
{
    vehicle::RawIMUDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawIMUDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        std::cerr << "ModularStateEstimator: rawUpdate_IMU: Invalid Event" 
                  << std::endl;
        return;
    }

    // /* Keep the most recent event from each IMU */
    // if(ievent->name == "MagBoom")
    //     rawBoomIMUDataEvent = ievent;
    // else
    //     rawIMUDataEvent = ievent;
    
    /* Update the estimated state by using an estimation module */
    imuEstimationModule->update(ievent, estimatedState);
}

void ModularStateEstimator::rawUpdate_DepthSensor(core::EventPtr event)
{
    vehicle::RawDepthSensorDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDepthSensorDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        std::cerr << "ModularStateEstimator: rawUpdate_DepthSensor: Invalid Event"
                  << std::endl;
        return;
    }   

    /* Update the estimated state by using an estimation module */
    depthEstimationModule->update(ievent, estimatedState);
}

void ModularStateEstimator::update_Vision(core::EventPtr event)
{
    std::cout << "ModularStateEstimator: rawUpdate_Vision: Placeholder" << std::endl;
}

void ModularStateEstimator::update_Sonar(core::EventPtr event)
{
    std::cout << "ModularStateEstimator: rawUpdate_Sonar: Placeholder" << std::endl;
}

} // namespace estimator
} // namespace ram

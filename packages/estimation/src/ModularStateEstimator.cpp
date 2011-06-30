/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/ModularStateEstimator.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "estimation/include/ModularStateEstimator.h"

#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IVelocitySensor.h"
#include "vehicle/include/Events.h"

#include "core/include/EventConnection.h"
#include "core/include/Event.h"
#include "core/include/SubsystemMaker.h"
#include "vision/include/Events.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::estimation::ModularStateEstimator,
                                  ModularStateEstimator);

namespace ram {
namespace estimation {

ModularStateEstimator::ModularStateEstimator(core::ConfigNode config, 
                                             core::EventHubPtr eventHub) :
    StateEstimatorBase(config,eventHub),
    dvlEstimationModule(EstimationModulePtr()),
    imuEstimationModule(EstimationModulePtr()),
    depthEstimationModule(EstimationModulePtr()),
    visionEstimationModule(EstimationModulePtr())
{


    // Construct the estimation modules
    dvlEstimationModule = EstimationModulePtr(
        new BasicDVLEstimationModule(config["DVLEstimationModule"],
                                     eventHub,
                                     estimatedState));

    imuEstimationModule = EstimationModulePtr(
        new IMUSGolayModule(config["IMUEstimationModule"],
                                     eventHub,
                                     estimatedState));

    depthEstimationModule = EstimationModulePtr(
        new DepthSGolayModule(config["DepthEstimationModule"],
                                       eventHub,
                                       estimatedState));
    
    //going to need to be more specific by making more modules for each eventType
    //below is simply an example of the (hopefully) correct format
    
    // visionEstimationModule = EstimationModulePtr(
    //     new VisionEstimationModule<vision::BuoyEvent>(vision::EventType::BUOY_FOUND,
    //                                                   eventHub,
    //                                                   config["VisionEstimationModule"],
    //                                                   estimatedState));
     
}

ModularStateEstimator::ModularStateEstimator(core::ConfigNode config, 
                                             core::SubsystemList deps) :
    StateEstimatorBase(config,deps),
    dvlEstimationModule(EstimationModulePtr()),
    imuEstimationModule(EstimationModulePtr()),
    depthEstimationModule(EstimationModulePtr())
{
    core::EventHubPtr eventHub = 
        core::Subsystem::getSubsystemOfType<core::EventHub>(deps);

    // Construct the estimation modules
    dvlEstimationModule = EstimationModulePtr(
        new BasicDVLEstimationModule(config["DVLEstimationModule"],
                                     eventHub,
                                     estimatedState));

    imuEstimationModule = EstimationModulePtr(
        new BasicIMUEstimationModule(config["IMUEstimationModule"],
                                     eventHub,
                                     estimatedState));

    depthEstimationModule = EstimationModulePtr(
        new DepthKalmanModule(config["DepthEstimationModule"],
                                       eventHub,
                                       estimatedState));

    /* need to create a BasicVisonEstimationModule class, has to implement 
     * the update method to update obstacles in the EstimatedState class, 
     * has to use string conversion method in the color class and other 
     * such methods to get the right names for the obstacles(string type)
     */
}


ModularStateEstimator::~ModularStateEstimator()
{    
}


} // namespace estimation
} // namespace ram

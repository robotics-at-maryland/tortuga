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

#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"
#include "math/include/Quaternion.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::estimation::ModularStateEstimator,
                                  ModularStateEstimator);

namespace ram {
namespace estimation {

ModularStateEstimator::ModularStateEstimator(core::ConfigNode config, 
                                             core::EventHubPtr eventHub) :
    StateEstimatorBase(config,eventHub),
    modules()
{
    init(config, eventHub);
}

ModularStateEstimator::ModularStateEstimator(core::ConfigNode config, 
                                             core::SubsystemList deps) :
    StateEstimatorBase(config,deps),
    modules()
{
    core::EventHubPtr eventHub = 
        core::Subsystem::getSubsystemOfType<core::EventHub>(deps);

    init(config, eventHub);
}


void ModularStateEstimator::init(core::ConfigNode config,
                                 core::EventHubPtr eventHub)
{
    //Removed 7/13/2013 to test Combined DVL Acclerometer Kalman filter
    //modules.push_back(EstimationModulePtr(
    //                      new BasicDVLEstimationModule(
    //                          config["DVLEstimationModule"],
    //                          eventHub,
    //                          m_estimatedState)));
    modules.push_back(EstimationModulePtr(
                          new DVLAccelerometerEstimator(
                              eventHub,
                              m_estimatedState)));

    modules.push_back(EstimationModulePtr(
                          new BasicIMUEstimationModule(
                              config["IMUEstimationModule"],
                              eventHub,
                              m_estimatedState)));

    modules.push_back(EstimationModulePtr(
                          new DepthKalmanModule(
                              config["DepthEstimationModule"],
                              eventHub,
                              m_estimatedState)));


    if(config.exists("GreenBuoy"))
    {
        addObstacle(config["GreenBuoy"], Obstacle::GREEN_BUOY);
        modules.push_back(
            EstimationModulePtr(new SimpleBuoyEstimationModule(
                                    config["GreenBuoyEstimationModule"],
                                    eventHub,
                                    m_estimatedState,
                                    Obstacle::GREEN_BUOY, 
                                    vision::EventType::BUOY_FOUND)));
    }

    if(config.exists("RedBuoy"))
    {
        addObstacle(config["RedBuoy"], Obstacle::RED_BUOY);
        modules.push_back(
            EstimationModulePtr(new SimpleBuoyEstimationModule(
                                    config["RedBuoyEstimationModule"],
                                    eventHub, 
                                    m_estimatedState,
                                    Obstacle::RED_BUOY,
                                    vision::EventType::BUOY_FOUND)));
    }

    if(config.exists("YellowBuoy"))
    {
        addObstacle(config["YellowBuoy"], Obstacle::YELLOW_BUOY);
        modules.push_back(
            EstimationModulePtr(new SimpleBuoyEstimationModule(
                                    config["YellowBuoyEstimationModule"],
                                    eventHub, 
                                    m_estimatedState,
                                    Obstacle::YELLOW_BUOY,
                                    vision::EventType::BUOY_FOUND)));
    }

}

void ModularStateEstimator::addObstacle(core::ConfigNode obstacleNode,
                                        Obstacle::ObstacleType type)
{
    ObstaclePtr obstacle = ObstaclePtr(new Obstacle());

    math::Vector3 location;
    location[0] = obstacleNode["location"][0].asDouble();
    location[1] = obstacleNode["location"][1].asDouble();
    location[2] = obstacleNode["location"][2].asDouble();

    math::Matrix3 covariance;
    covariance[0][0] = obstacleNode["covariance"][0][0].asDouble(5);
    covariance[0][1] = obstacleNode["covariance"][0][1].asDouble(0);
    covariance[0][2] = obstacleNode["covariance"][0][2].asDouble(0);
    covariance[1][0] = obstacleNode["covariance"][1][0].asDouble(0);
    covariance[1][1] = obstacleNode["covariance"][1][1].asDouble(5);
    covariance[1][2] = obstacleNode["covariance"][1][2].asDouble(0);
    covariance[2][0] = obstacleNode["covariance"][2][0].asDouble(0);
    covariance[2][1] = obstacleNode["covariance"][2][1].asDouble(0);
    covariance[2][2] = obstacleNode["covariance"][2][2].asDouble(1);

    double heading = obstacleNode["attackHeading"].asDouble(0);
    math::Quaternion attackOrientation = math::Quaternion(math::Degree(heading), math::Vector3::UNIT_Z);

    obstacle->setLocation(location);
    obstacle->setLocationCovariance(covariance);
    obstacle->setAttackOrientation(attackOrientation);

    m_estimatedState->addObstacle(type, obstacle);
}

} // namespace estimation
} // namespace ram

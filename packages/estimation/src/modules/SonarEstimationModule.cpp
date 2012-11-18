/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/modules/SonarEstimationModule.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/Events.h"
#include "estimation/include/modules/SonarEstimationModule.h"
#include "math/include/Vector3.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstSonar"));

namespace ram {
namespace estimation {


SonarEstimationModule::SonarEstimationModule(
    core::ConfigNode config,
    core::EventHubPtr eventHub,
    EstimatedStatePtr estState) :
    EstimationModule(eventHub, "SonarEstimationModule", estState,
                     vehicle::device::SensorBoard::SONAR_UPDATE),
    m_numMeasurements(config["numMeasurements"].asInt())
{
    LOGGER.info("% Namee[1] Direction[3] Range[1] PingTimeSec[1] PingTimeUSec[1] PingCount[1] PingerID[1]");
}

void SonarEstimationModule::update(core::EventPtr event)
{
    /* Attempt to cast the event to a RawDepthSensorDataEventPtr */
    vehicle::SonarEventPtr sonarEvent =
        boost::dynamic_pointer_cast<vehicle::SonarEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!sonarEvent){
        LOGGER.warn("BasicSonarEstimationModule: update: Invalid Event Type");
        return;
    }

    // this is a direction relative to the body coordinates since the hydrophone
    // array has no clue what the current orientation of the robot is
    math::Vector3 direction_b = sonarEvent->direction;
    double range = sonarEvent->range;

    math::Quaternion orientation = m_estimatedState->getEstimatedOrientation();
    math::Vector3 direction_n = orientation.UnitInverse() * direction_b;

    // the range is independent of any coordinate frame so to get the error vector
    // from the vehicle to the sonar pinger we multiply by the range
    math::Vector3 robotToPinger = direction_n * range;

    // we want the absolute position of the pinger so we need to subtract the
    // current position estimation
    math::Vector2 position = m_estimatedState->getEstimatedPosition();
    double depth = m_estimatedState->getEstimatedDepth();
    math::Vector3 robotLocation(position[0], position[1], depth);
    math::Vector3 pingerLocation = robotToPinger - robotLocation;
    
    m_pingerLocations.push_back(pingerLocation);

    // we're going to estimated the covarinace by computing the covarinance of a series
    // of measurements since we are estimating the absolute location of the pinger
    // each time.
    m_estimatedState->setObstacleLocation(Obstacle::PINGER, pingerLocation);

    // going to implement the covariance thing later
    //m_estimatedState->setObstacleCovariance(Obstacle::PINGER, covariance);

    ObstacleEventPtr obstacleEvent = ObstacleEventPtr(new ObstacleEvent());
    obstacleEvent->obstacle = Obstacle::PINGER;
    obstacleEvent->location = pingerLocation;
    obstacleEvent->covariance = math::Matrix3::IDENTITY;
        
}


} // namespace estimation
} // namespace ram

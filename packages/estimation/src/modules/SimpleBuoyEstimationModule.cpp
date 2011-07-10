/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/src/modules/ParticleBuoyEstimationModule.cpp
 */

// STD Includes
#include <cmath>
#include <algorithm>
#include <iostream>

// Library Includes
#include <boost/foreach.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "estimation/include/modules/SimpleBuoyEstimationModule.h"
#include "estimation/include/Events.h"
#include "vision/include/Events.h"
#include "math/include/Helpers.h"
#include "math/include/Events.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstBuoy"));


namespace ram {
namespace estimation {

SimpleBuoyEstimationModule::SimpleBuoyEstimationModule(
    core::ConfigNode config,
    core::EventHubPtr eventHub,
    EstimatedStatePtr estState,
    Obstacle::ObstacleType obstacle,
    core::Event::EventType inputEventType) :
    EstimationModule(eventHub, "SimpleBuoyEstimationModule",
                     estState, inputEventType),
    m_obstacle(obstacle),
    m_initialGuess(estState->getObstacleLocation(obstacle)),
    m_initialUncertainty(estState->getObstacleLocationCovariance(obstacle)),
    m_intrinsicParameters(math::Matrix3::IDENTITY),
    m_numMeasurements(config["numMeasurements"].asInt(30))
{
    // get the camera intrinsic paramters
    math::Radian m_xFOV = vision::VisionSystem::getFrontHorizontalFieldOfView();
    math::Radian m_yFOV = vision::VisionSystem::getFrontVerticalFieldOfView();

    double m_camWidth = vision::VisionSystem::getFrontHorizontalPixelResolution();
    double m_camHeight = vision::VisionSystem::getFrontVerticalPixelResolution();

    double cameraFocalX = m_camWidth / std::tan(m_xFOV.valueRadians() / 2.0);
    double cameraFocalY = m_camHeight / std::tan(m_yFOV.valueRadians() / 2.0);

    m_intrinsicParameters = math::Matrix3(cameraFocalX, 0, 0,
                                          0, cameraFocalY, 0,
                                          0, 0, 1);

    m_invIntrinsicParameters = math::Matrix3(cameraFocalX, 0, 0,
                                             0, cameraFocalY, 0,
                                             0, 0, 1);

    m_buoyMeasurements.push_back(m_initialGuess);
}


void SimpleBuoyEstimationModule::update(core::EventPtr event)
{
    vision::BuoyEventPtr buoyEvent = 
        boost::dynamic_pointer_cast<vision::BuoyEvent>(event);

    if(!buoyEvent)
    {
        std::cerr << "Invalid Event Type" << std::endl;
        return;
    }
    
    // get the info out of the event
    double xPixelCoord = buoyEvent->x * m_camWidth;
    double yPixelCoord = buoyEvent->y * m_camHeight;
    double distance = buoyEvent->range;
    math::Vector2 objImageCoords(xPixelCoord, yPixelCoord);

    // get the current estimated state
    math::Vector2 estPosition = m_estimatedState->getEstimatedPosition();
    double estDepth = m_estimatedState->getEstimatedDepth();
    math::Vector3 estCameraLocation(estPosition[0], estPosition[1], estDepth);
    math::Quaternion estOrientation = m_estimatedState->getEstimatedOrientation();

    math::Vector3 buoyLocation = img2world(objImageCoords,
                                           distance,
                                           estCameraLocation,
                                           estOrientation,
                                           m_intrinsicParameters);

    m_buoyMeasurements.push_back(buoyLocation);

    while(m_buoyMeasurements.size() > m_numMeasurements)
        m_buoyMeasurements.pop_front();


    math::Vector3 bestEstimate = getBestEstimate();
    math::Matrix3 covariance = getCovariance();

    m_estimatedState->setObstacleLocation(m_obstacle, bestEstimate);
    m_estimatedState->setObstacleLocationCovariance(m_obstacle, covariance);

    ObstacleEventPtr obstacleEvent = ObstacleEventPtr(new ObstacleEvent());
    obstacleEvent->obstacle = m_obstacle;
    obstacleEvent->location = bestEstimate;
    obstacleEvent->covariance = covariance;

    publish(IStateEstimator::ESTIMATED_OBSTACLE_UPDATE, obstacleEvent);
}

math::Vector3 SimpleBuoyEstimationModule::getBestEstimate()
{
    double sumX = 0, sumY = 0, sumZ = 0;
    int n = m_buoyMeasurements.size();

    // compute the weighted average for each coordinate
    BOOST_FOREACH(math::Vector3 measurement, m_buoyMeasurements)
    {
        sumX += measurement[0];
        sumY += measurement[1];
        sumZ += measurement[2];
    }
    return math::Vector3(sumX / n, sumY / n, sumZ / n);
}


math::Matrix3 SimpleBuoyEstimationModule::getCovariance()
{
    size_t n = m_buoyMeasurements.size();

    math::Vector3 expValues = math::Vector3::ZERO;
    BOOST_FOREACH(math::Vector3 &measurement, m_buoyMeasurements)
    {
        expValues += measurement;
    }
    expValues /= n;
    
    math::Matrix3 covariance(math::Matrix3::ZERO);
    BOOST_FOREACH(math::Vector3 &measurement, m_buoyMeasurements)
    {
        for(int x = 0; x < 3; x++)
        {
            for(int y = 0; y < 3; y++)
            {
                double xval = measurement[x] - expValues[x];
                double yval = measurement[y] - expValues[y];
                covariance[x][y] += (xval * yval / n);
            }
        }
    }
    return covariance;
}

} // namespace estimation
} // namespace ram

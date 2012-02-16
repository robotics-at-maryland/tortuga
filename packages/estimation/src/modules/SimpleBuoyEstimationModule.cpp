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
#include "vision/include/Color.h"
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

    // this is ugly, but its the easiest way of dealing with having the same
    // buoy found event type for all colors
    if(buoyEvent->color == vision::Color::YELLOW && 
       m_obstacle != Obstacle::YELLOW_BUOY)
        return;

    if(buoyEvent->color == vision::Color::GREEN && 
       m_obstacle != Obstacle::GREEN_BUOY)
        return;

    if(buoyEvent->color == vision::Color::RED && 
       m_obstacle != Obstacle::RED_BUOY)
        return;

    // if the buoy is touching the edge the distance estimate will be dangerous
    // we want to avoid this situation even if it means ignoring a found event
    if(buoyEvent->touchingEdge)
        return;
    
    // get the info out of the event
    math::Vector3 measurement_i(buoyEvent->x * m_camWidth,
                                buoyEvent->y * m_camHeight,
                                buoyEvent->range);

    // get the current estimated state
    math::Vector2 robotPosition = m_estimatedState->getEstimatedPosition();
    double robotDepth = m_estimatedState->getEstimatedDepth();

    // TODO: need to get offsets for how far camera is from cg
    // should be config values
    math::Vector3 cameraLocation(robotPosition[0], robotPosition[1], robotDepth);
    math::Quaternion cameraOrientation = m_estimatedState->getEstimatedOrientation();

    math::Vector3 measurement_w = img2world(measurement_i,
                                            cameraLocation,
                                            cameraOrientation,
                                            m_intrinsicParameters);

    m_buoyMeasurements.push_back(measurement_w);

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

    LOGGER.infoStream() << measurement_i[0] << " "
                        << measurement_i[1] << " "
                        << measurement_i[2] << " "
                        << measurement_w[0] << " "
                        << measurement_w[1] << " "
                        << measurement_w[2] << " "
                        << bestEstimate[0] << " "
                        << bestEstimate[1] << " "
                        << bestEstimate[2];
}

math::Vector3 SimpleBuoyEstimationModule::getBestEstimate()
{
    double sumX = 0, sumY = 0, sumZ = 0;
    int n = m_buoyMeasurements.size();

    // compute the average for each coordinate
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

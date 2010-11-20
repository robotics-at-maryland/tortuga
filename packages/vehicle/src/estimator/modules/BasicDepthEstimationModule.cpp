/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/modules/BasicDepthEstimationModule.cpp
 */

// Library Includes
#include <iostream>
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "vehicle/include/estimator/modules/BasicDepthEstimationModule.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstDepth"));

namespace ram {
namespace estimator {

BasicDepthEstimationModule::BasicDepthEstimationModule(core::ConfigNode config,
                                                       core::EventHubPtr eventHub) :
    EstimationModule(eventHub, "BasicDepthEstimationModule"),
    m_location(math::Vector3::ZERO),
    m_calibSlope(0),
    m_calibIntercept(0)
{
    /* initialization of estimator from config values should be done here */
    LOGGER.info("% Name EstDepth RawDepth Correction");
}


void BasicDepthEstimationModule::init(core::EventPtr event)
{
    // receive the sensor config parameters
    vehicle::DepthSensorInitEventPtr ievent = 
        boost::dynamic_pointer_cast<vehicle::DepthSensorInitEvent>(event);

    if(!event) {
        LOGGER.warn("BasicDepthEstimationModule: init: Invalid EventType");
        return;
    } else {
        LOGGER.info("BasicIMUEstimationModule: init: Config Received "
                    + ievent->name);
    }

    m_name = ievent->name;
    m_location = ievent->location;
    m_calibSlope = ievent->depthCalibSlope;
    m_calibIntercept = ievent->depthCalibIntercept;
}

void BasicDepthEstimationModule::update(core::EventPtr event, 
                                        EstimatedStatePtr estimatedState)
{
    /* Attempt to cast the event to a RawDepthSensorDataEventPtr */
    vehicle::RawDepthSensorDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDepthSensorDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        LOGGER.warn("BasicDepthEstimationModule: update: Invalid Event Type");
        return;
    }

    /* This is where the estimation should be done
       The result should be stored in estimatedState */

    // Determine depth correction
    math::Vector3 currentSensorLocation = 
      estimatedState->getEstimatedOrientation() * m_location;
    math::Vector3 sensorMovement = 
      currentSensorLocation - m_location;
    double correction = sensorMovement.z;
    

    // Grab the depth
    double depth = ievent->rawDepth;

    // Return the corrected depth (its addition and not subtraction because
    // depth is positive down)
    estimatedState->setEstimatedDepth(depth + correction);

    LOGGER.infoStream() << m_name << " "
                        << depth + correction << " "
                        << depth << " "
                        << correction;
}

} // namespace estimator
} // namespace ram

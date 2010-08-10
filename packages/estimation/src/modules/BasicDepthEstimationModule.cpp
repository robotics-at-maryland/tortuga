/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/modules/BasicDepthEstimationModule.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <boost/smart_cast.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/modules/BasicDepthEstimationModule.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstDepth"));

namespace ram {
namespace estimation {

BasicDepthEstimationModule::BasicDepthEstimationModule(
    core::ConfigNode config,
    core::EventHubPtr eventHub) :
    EstimationModule(eventHub, "BasicDepthEstimationModule")
{
    /* initialization of estimator from config values should be done here */
    LOGGER.info("% Name EstDepth RawDepth Correction");
}

void BasicDepthEstimationModule::update(
    core::EventPtr event, 
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
    math::Vector3 location = ievent->sensorLocation;
    math::Vector3 currentSensorLocation = 
        estimatedState->getEstOrientation() * location;
    math::Vector3 sensorMovement = 
        currentSensorLocation - location;
    double correction = sensorMovement.z;
    

    // Grab the depth
    double depth = ievent->rawDepth;

    /* Return the corrected depth (its addition and not subtraction because
     * depth is positive down) */

    estimatedState->setEstDepth(depth + correction);

    LOGGER.infoStream() << m_name << " "
                        << depth + correction << " "
                        << depth << " "
                        << correction;
}

} // namespace estimation
} // namespace ram

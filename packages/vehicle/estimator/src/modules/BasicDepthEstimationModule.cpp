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
#include <boost/smart_cast.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "vehicle/estimator/include/modules/BasicDepthEstimationModule.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstDepth"));

namespace ram {
namespace estimator {

BasicDepthEstimationModule::BasicDepthEstimationModule(core::ConfigNode config) :
    m_location(math::Vector3::ZERO),
    m_calibSlope(0),
    m_calibIntercept(0)
{
    /* initialization from config values should be done here */

    LOGGER.info("% Name EstDepth");
}


void BasicDepthEstimationModule::init(core::EventPtr event)
{
    // receive the sensor config parameters
    vehicle::DepthSensorInitEventPtr ievent = 
        boost::dynamic_pointer_cast<vehicle::DepthSensorInitEvent>(event);

    if(!event) {
        std::cerr << "BasicDepthEstimationModule: init: Invalid Event Type"
                  << std::endl; 
        return;
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
        std::cerr << "BasicDepthEstimationModule: update: Invalid Event Type" 
                  << std::endl;
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
                        << depth + correction;
}

} // namespace estimatior
} // namespace ram

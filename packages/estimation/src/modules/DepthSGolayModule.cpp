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
#include <boost/shared_ptr.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/modules/DepthSGolayModule.h"
#include "vehicle/include/device/IDepthSensor.h"


static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstDepth"));

namespace ram {
namespace estimation {

DepthSGolayModule::DepthSGolayModule(
    core::ConfigNode config,
    core::EventHubPtr eventHub, EstimatedStatePtr estState) :
    EstimationModule(eventHub, "DepthSGolayModule",estState,
                     vehicle::device::IDepthSensor::RAW_UPDATE),
    m_degree(2),
    m_window(25),
    m_filteredDepth(core::SGolaySmoothingFilter(m_degree, m_window))
{
    /* initialization of estimator from config values should be done here */
    LOGGER.info("% RawDepth Correction EstDepth EstDepthRate");

}

void DepthSGolayModule::update(
    core::EventPtr event)
{
    /* Attempt to cast the event to a RawDepthSensorDataEventPtr */
    vehicle::RawDepthSensorDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDepthSensorDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        LOGGER.warn("DepthSGolayModule: update: Invalid Event Type");
        return;
    }

    /* This is where the estimation should be done
       The result should be stored in m_estimatedState */

    // Determine depth correction
    math::Vector3 location = ievent->sensorLocation;
    math::Vector3 currentSensorLocation = 
        m_estimatedState->getEstimatedOrientation() * location;
    math::Vector3 sensorMovement = currentSensorLocation - location;
    double correction = sensorMovement.z;

    // grab the depth and calculate the depth rate
    double rawDepth = ievent->rawDepth;
    double depth = rawDepth + correction;

    // put the depth into the averaging filter
    m_filteredDepth.addValue(depth);
   

    /* Return the corrected depth (its addition and not subtraction because
     * depth is positive down) */

    double estDepth = m_filteredDepth.getValue();
    double estDepthRate = m_filteredDepth.getDerivative();

    m_estimatedState->setEstimatedDepth(estDepth);
    m_estimatedState->setEstimatedDepthRate(estDepthRate);

    LOGGER.infoStream() << rawDepth<< " "
                        << correction << " "
                        << estDepth << " "
                        << estDepthRate;
}

} // namespace estimation
} // namespace ram

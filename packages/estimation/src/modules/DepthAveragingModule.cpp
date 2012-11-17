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
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/modules/DepthAveragingModule.h"
#include "vehicle/include/device/IDepthSensor.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstDepth"));

namespace ram {
namespace estimation {

DepthAveragingModule::DepthAveragingModule(
    core::ConfigNode config,
    core::EventHubPtr eventHub,EstimatedStatePtr estState) :
    EstimationModule(eventHub, "DepthAveragingModule",estState,
                     vehicle::device::IDepthSensor::RAW_UPDATE)
{
    // initialization of estimator from config values should be done here
    LOGGER.info("% RawDepth Correction EstDepth EstDepthRate");
}

void DepthAveragingModule::update(
    core::EventPtr event)
{
    // attempt to cast the event to a RawDepthSensorDataEventPtr
    vehicle::RawDepthSensorDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDepthSensorDataEvent>(event);

    // return if the cast failed and let people know about it.
    if(!ievent){
        LOGGER.warn("DepthAveragingModule: update: Invalid Event Type");
        return;
    }

    // This is where the estimation should be done
    // The result should be stored in estimatedState

    // determine depth correction
    math::Vector3 location = ievent->sensorLocation;
    math::Vector3 currentSensorLocation = 
        m_estimatedState->getEstimatedOrientation() * location;
    math::Vector3 sensorMovement = currentSensorLocation - location;
    double correction = sensorMovement.z;

    double timestep = ievent->timestep;

    // grab the depth and calculate the depth rate
    double rawDepth = ievent->rawDepth;
    double depth = rawDepth + correction;
    double depthRate = (depth - m_previousDepth) / timestep;

    // put the depth into the averaging filter
    m_filteredDepth.addValue(depth);
    m_filteredDepthRate.addValue(depthRate);

    // return the corrected depth (its addition and not subtraction because
    // depth is positive down)

    double estDepth = m_filteredDepth.getValue();
    double estDepthRate = m_filteredDepthRate.getValue();

    m_estimatedState->setEstimatedDepth(estDepth);
    m_estimatedState->setEstimatedDepthRate(estDepthRate);

    LOGGER.infoStream() << rawDepth<< " "
                        << correction << " "
                        << estDepth << " "
                        << estDepthRate;
}

} // namespace estimation
} // namespace ram

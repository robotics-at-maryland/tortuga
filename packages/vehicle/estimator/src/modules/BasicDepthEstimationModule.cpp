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

// Project Includes
#include "vehicle/include/Events.h"
#include "vehicle/estimator/include/modules/BasicDepthEstimationModule.h"

namespace ram {
namespace estimator {

BasicDepthEstimationModule::BasicDepthEstimationModule(core::ConfigNode config)
{
    /* initialization from config values should be done here */
}


void BasicDepthEstimationModule::init(core::EventPtr event)
{

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

    // Temporarily ignoring correction until orientation estimator implemented
    /*
    // Determine depth correction
    math::Vector3 initialSensorLocation = ievent->sensorLocation;
    math::Vector3 currentSensorLocation = 
      estimatedState->getEstimatedOrientation() * initialSensorLocation;
    math::Vector3 sensorMovement = 
      currentSensorLocation - initialSensorLocation;
    double correction = sensorMovement.z;
    */

    // Grab the depth
    double depth = ievent->rawDepth;

    // Return the corrected depth (its addition and not subtraction because
    // depth is positive down)
    // correction not used right now because the new state estimator doesnt
    // handle the orientation estimation yet
    estimatedState->setEstimatedDepth(depth);// + correction);
}

} // namespace estimatior
} // namespace ram

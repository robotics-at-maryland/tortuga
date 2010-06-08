/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/modules/BasicDVLEstimationModule.cpp
 */

// Library Includes
#include <iostream>
#include <boost/smart_cast.hpp>

// Project Includes
#include "vehicle/include/Events.h"
#include "vehicle/estimator/include/modules/BasicDVLEstimationModule.h"

namespace ram {
namespace estimator {

BasicDVLEstimationModule::BasicDVLEstimationModule(core::ConfigNode config)
{
    /* initialization from config values should be done here */
}

void BasicDVLEstimationModule::init(core::EventPtr event)
{

}

void BasicDVLEstimationModule::update(core::EventPtr event, 
                                      EstimatedStatePtr estimatedState)
{

    vehicle::RawDVLDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDVLDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        std::cout << "BasicDVLEstimationModule: update: Invalid Event Type" 
                  << std::endl;
        return;
    }

    /* This is where the estimation should be done
       The result should be stored in estimatedState */

}

} // namespace estimatior
} // namespace ram

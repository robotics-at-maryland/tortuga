/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/EstimationModule.cpp
 */

// Library Includes
#include <string.h>

// Package Includes
#include "vehicle/estimator/include/EstimationModule.h"


namespace ram {
namespace estimator {

EstimationModule::EstimationModule(core::EventHubPtr eventHub,
                                   std::string name) :
    core::EventPublisher(eventHub, name)
{
}

} // namespace estimator
} // namespace ram

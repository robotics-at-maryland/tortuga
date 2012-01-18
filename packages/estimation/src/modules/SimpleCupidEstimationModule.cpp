/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/src/modules/ParticleCupidEstimationModule.cpp
 */

// STD Includes
#include <cmath>
#include <algorithm>
#include <iostream>

// Library Includes
#include <boost/foreach.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "estimation/include/modules/SimpleCupidEstimationModule.h"
#include "estimation/include/Events.h"
#include "vision/include/Events.h"
#include "math/include/Helpers.h"
#include "math/include/Events.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstCupid"));


namespace ram {
namespace estimation {

SimpleCupidEstimationModule::SimpleCupidEstimationModule(
    core::ConfigNode config,
    core::EventHubPtr eventHub,
    EstimatedStatePtr estState,
    Obstacle::ObstacleType obstacle,
    core::Event::EventType inputEventType) :
    EstimationModule(eventHub, "SimpleCupidEstimationModule",
                     estState, inputEventType)
{
}

void SimpleCupidEstimationModule::update(core::EventPtr event)
{
}

} // namespace estimation
} // namespace ram

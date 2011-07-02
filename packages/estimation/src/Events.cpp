/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/src/Events.h
 */

// STD Includes

// Library Includes

// Project Includes
#include "estimation/include/Events.h"

namespace ram {
namespace estimation {

core::EventPtr ObstacleEvent::clone()
{
    ObstacleEventPtr event = ObstacleEventPtr(new ObstacleEvent());
    copyInto(event);
    event->obstacle = obstacle;
    event->location = location;
    event->covariance = covariance;
    return event;
}

} // namespace estimation
} // namespace ram

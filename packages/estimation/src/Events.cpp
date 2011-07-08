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

#if defined(RAM_WITH_WRAPPERS)
#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::estimation::ObstacleEvent>
RAM_ESTIMATION_OBSTACLEEVENT;

#endif //RAM_WITH_WRAPPERS

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

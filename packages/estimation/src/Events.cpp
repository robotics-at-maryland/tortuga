/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/Events.cpp
 */

// Project Includes
#include "core/include/Feature.h"
#include "estimation/include/Events.h"

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS)

#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::estimation::ThrustUpdateEvent>
RAM_ESTIMATION_THRUST_UPDATE_EVENT;


#endif // RAM_WITH_WRAPPERS


namespace ram {
namespace estimation {

core::EventPtr ThrustUpdateEvent::clone()
{
    ThrustUpdateEventPtr event = ThrustUpdateEventPtr(new ThrustUpdateEvent());
    copyInto(event);

    event->forces = forces;
    event->torques = torques;

    return event;
}


} // namespace estimation
} // namespace ram

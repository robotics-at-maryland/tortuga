/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/Events.h
 */

#ifndef RAM_ESTIMATION_EVENTS_H
#define RAM_ESTIMATION_EVENTS_H


// Project Includes
#include "core/include/Event.h"
#include "math/include/Vector3.h"

namespace ram {
namespace estimation {

struct ThrustUpdateEvent : public core::Event
{
    math::Vector3 forces;
    math::Vector3 torques;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<ThrustUpdateEvent> ThrustUpdateEventPtr;

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_EVENTS_H

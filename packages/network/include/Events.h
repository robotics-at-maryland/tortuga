/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/include/Events.h
 */

#ifndef RAM_NETWORK_EVENTS_02_23_2011
#define RAM_NETWORK_EVENTS_02_23_2011

// Project Includes
#include "core/include/Event.h"

namespace ram {
namespace network {

struct EventType
{
    static const core::Event::EventType EMERGENCY_STOP;
    static const core::Event::EventType YAW_LEFT;
    static const core::Event::EventType YAW_RIGHT;
    static const core::Event::EventType PITCH_UP;
    static const core::Event::EventType PITCH_DOWN;
    static const core::Event::EventType ROLL_LEFT;
    static const core::Event::EventType ROLL_RIGHT;
    static const core::Event::EventType FORWARD_MOVEMENT;
    static const core::Event::EventType DOWNWARD_MOVEMENT;
    static const core::Event::EventType LEFT_MOVEMENT;
    static const core::Event::EventType RIGHT_MOVEMENT;
    static const core::Event::EventType DESCEND;
    static const core::Event::EventType ASCEND;
    static const core::Event::EventType SETSPEED;
    static const core::Event::EventType TSETSPEED;
    static const core::Event::EventType ANGLEYAW;
    static const core::Event::EventType ANGLEPITCH;
    static const core::Event::EventType ANGLEROLL;
    static const core::Event::EventType FIRE_MARKER_DROPPER;
    static const core::Event::EventType FIRE_TORPEDO_LAUNCHER;
};

} // namespace network
} // namespace ram

#endif // RAM_NETWORK_EVENTS_02_23_2011

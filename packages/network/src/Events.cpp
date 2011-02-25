/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/src/Events.cpp
 */

// Project Includes
#include "network/include/Events.h"

RAM_CORE_EVENT_TYPE(ram::network::EventType, EMERGENCY_STOP);
RAM_CORE_EVENT_TYPE(ram::network::EventType, YAW_LEFT);
RAM_CORE_EVENT_TYPE(ram::network::EventType, YAW_RIGHT);
RAM_CORE_EVENT_TYPE(ram::network::EventType, PITCH_UP);
RAM_CORE_EVENT_TYPE(ram::network::EventType, PITCH_DOWN);
RAM_CORE_EVENT_TYPE(ram::network::EventType, ROLL_LEFT);
RAM_CORE_EVENT_TYPE(ram::network::EventType, ROLL_RIGHT);
RAM_CORE_EVENT_TYPE(ram::network::EventType, FORWARD_MOVEMENT);
RAM_CORE_EVENT_TYPE(ram::network::EventType, DOWNWARD_MOVEMENT);
RAM_CORE_EVENT_TYPE(ram::network::EventType, LEFT_MOVEMENT);
RAM_CORE_EVENT_TYPE(ram::network::EventType, RIGHT_MOVEMENT);
RAM_CORE_EVENT_TYPE(ram::network::EventType, ASCEND);
RAM_CORE_EVENT_TYPE(ram::network::EventType, DESCEND);
RAM_CORE_EVENT_TYPE(ram::network::EventType, SETSPEED);
RAM_CORE_EVENT_TYPE(ram::network::EventType, TSETSPEED);
RAM_CORE_EVENT_TYPE(ram::network::EventType, ANGLEYAW);
RAM_CORE_EVENT_TYPE(ram::network::EventType, ANGLEPITCH);
RAM_CORE_EVENT_TYPE(ram::network::EventType, ANGLEROLL);
RAM_CORE_EVENT_TYPE(ram::network::EventType, FIRE_MARKER_DROPPER);
RAM_CORE_EVENT_TYPE(ram::network::EventType, FIRE_TORPEDO_LAUNCHER);

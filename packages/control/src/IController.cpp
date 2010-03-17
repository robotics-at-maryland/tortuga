/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/IController.cpp
 */

// Project Includes
#include "control/include/IController.h"

RAM_CORE_EVENT_TYPE(ram::control::IController, DESIRED_DEPTH_UPDATE);
RAM_CORE_EVENT_TYPE(ram::control::IController, DESIRED_ORIENTATION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::control::IController, DESIRED_VELOCITY_UPDATE);
RAM_CORE_EVENT_TYPE(ram::control::IController, DESIRED_POSITION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::control::IController, AT_DEPTH);
RAM_CORE_EVENT_TYPE(ram::control::IController, AT_ORIENTATION);
RAM_CORE_EVENT_TYPE(ram::control::IController, AT_VELOCITY);
RAM_CORE_EVENT_TYPE(ram::control::IController, AT_POSITION);
RAM_CORE_EVENT_TYPE(ram::control::IController, PARAM_SETUP);
RAM_CORE_EVENT_TYPE(ram::control::IController, PARAM_UPDATE);

namespace ram {
namespace control {

IController::IController(std::string name, core::EventHubPtr eventHub) :
    core::Subsystem(name, eventHub)
{
}

} // namespace control
} // namespace ram

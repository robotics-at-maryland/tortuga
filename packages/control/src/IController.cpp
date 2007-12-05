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

namespace ram {
namespace control {

IController::IController(std::string name) : 
    Subsystem(name) 
{
}

} // namespace control
} // namespace ram

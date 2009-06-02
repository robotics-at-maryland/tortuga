/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Event.cpp
 */

// Project Includes
#include "core/include/Event.h"
#include "core/include/TimeVal.h"

namespace ram {
namespace core {

Event::Event() :
    timeStamp(TimeVal::timeOfDay().get_double())
{
}

} // namespace core
} // namespace ram

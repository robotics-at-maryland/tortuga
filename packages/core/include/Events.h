/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Events.h
 */

#ifndef RAM_CORE_EVENTS_06_04_2009
#define RAM_CORE_EVENTS_06_04_2009

// Project Includes
#include "core/include/Event.h"

namespace ram {
namespace core {

struct StringEvent : public core::Event
{
    std::string string;
};

typedef boost::shared_ptr<StringEvent> StringEventPtr;
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_EVENTS_06_04_2009

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
    sender(0),
    timeStamp(TimeVal::timeOfDay().get_double())
{
}

EventPtr Event::clone()
{
    EventPtr event = EventPtr(new Event());
    copyInto(event);
    return event;
}

void Event::copyInto(EventPtr inEvent)
{
    inEvent->type = type;
    inEvent->sender = sender;
    inEvent->timeStamp = timeStamp;
}    
    
} // namespace core
} // namespace ram

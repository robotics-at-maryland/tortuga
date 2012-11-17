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

// STD Includes
#include <vector>
#include <string>

// Project Includes
#include "core/include/Event.h"
#include "math/include/Vector2.h"

namespace ram {
namespace core {

struct StringEvent : public core::Event
{
    std::string string;

    virtual EventPtr clone();
};

typedef boost::shared_ptr<StringEvent> StringEventPtr;

struct IntEvent : public core::Event
{
    IntEvent() : data(0) {}

    IntEvent(int data_) : data(data_) {}

    virtual EventPtr clone()
    {
        boost::shared_ptr< IntEvent > event =
            boost::shared_ptr< IntEvent >(new IntEvent());
        copyInto(event);
        event->data = data;
        return event;
    }

    int data;
};

typedef boost::shared_ptr< IntEvent > IntEventPtr;

} // namespace core
} // namespace ram

#endif // RAM_CORE_EVENTS_06_04_2009

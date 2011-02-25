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

namespace ram {
namespace core {

struct StringEvent : public core::Event
{
    std::string string;

    virtual EventPtr clone();
};

typedef boost::shared_ptr<StringEvent> StringEventPtr;

template <typename DataType>
struct DataEvent : public core::Event
{
    DataEvent() : data(0) {}

    DataEvent(int data_) : data(data_) {}

    virtual EventPtr clone()
    {
        boost::shared_ptr< DataEvent<DataType> > event =
            boost::shared_ptr< DataEvent<DataType> >(new DataEvent<DataType>());
        copyInto(event);
        event->data = data;
        return event;
    }

    DataType data;
};

typedef DataEvent<int> IntEvent;
typedef boost::shared_ptr< IntEvent > IntEventPtr;

typedef DataEvent<bool> BoolEvent;
typedef boost::shared_ptr< BoolEvent > BoolEventPtr;

} // namespace core
} // namespace ram

#endif // RAM_CORE_EVENTS_06_04_2009

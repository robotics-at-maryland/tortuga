/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Event.h
 */

#ifndef RAM_CORE_EVENT_H_11_19_2007
#define RAM_CORE_EVENT_H_11_19_2007

// STD Includes
#include <string>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/Forward.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

/** Base class for information to be passed between publisher and subscriber */
struct RAM_EXPORT Event
{
    typedef std::string EventType;
    
    Event();
    
    virtual ~Event() {};

    /** Provides a deep copy of the event object */
    virtual EventPtr clone();

    /** A unique identifier for a stream of events from an EventPublisher */
    EventType type;

    /** The original source who published the event */
    EventPublisher* sender;

    /** The time at which the event was created
     *
     *  The time is in seconds since the start of the UNIX epoch.  Percision is
     *  approximately microseconds
     */
    double timeStamp;

  protected:
    /** Copies all elements of the event into the given event */
    void copyInto(EventPtr inEvent);
};
    
} // namespace core
} // namespace ram

#define RAM_CORE_EVENT_STRINGIFY(S) #S
#define RAM_CORE_EVENT_STR(S) RAM_CORE_EVENT_STRINGIFY(S)
#define RAM_CORE_EVENT_TYPE(_class, name) \
    const ram::core::Event::EventType _class  :: name  \
    (RAM_CORE_EVENT_STR(__LINE__) " " RAM_CORE_EVENT_STR(_class) "::" \
     RAM_CORE_EVENT_STR(name))

#endif // RAM_CORE_EVENT_H_11_19_2007

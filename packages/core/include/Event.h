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

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class EventPublisher;
    
struct RAM_EXPORT Event
{
    typedef std::string EventType;
    
    EventType type;
    EventPublisher* sender;
};

typedef boost::shared_ptr<Event> EventPtr;

/*namespace details {
int instantiateEvent() {
    return sizeof(EventPtr);
}
}*/
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_EVENT_H_11_19_2007

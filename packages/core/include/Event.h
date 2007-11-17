/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Event.h
 */

// Library Includes
#include <boost/shared_ptr.hpp>


namespace ram {
namespace core {

class EventPublisher;
    
struct Event
{
    typedef std::string EventType;
    
    EventType type;
    EventPublisher* sender;
};

typedef boost::shared_ptr<Event> EventPtr;
    
} // namespace core
} // namespace ram

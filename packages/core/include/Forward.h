/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Forward.h
 */

#ifndef RAM_CORE_FORWARD_H_11_30_2007
#define RAM_CORE_FORWARD_H_11_30_2007

namespace ram {
namespace core {

// Library Includes
#include <boost/shared_ptr.hpp>

struct Event;
typedef boost::shared_ptr<Event> EventPtr;

class EventConnection;
typedef boost::shared_ptr<EventConnection> EventConnectionPtr;
    
class EventPublisher;
    
class EventPublisherBase; 
typedef boost::shared_ptr<EventPublisherBase> EventPublisherBasePtr;

class QueuedEventPublisherBase; 
typedef boost::shared_ptr<QueuedEventPublisherBase> QueuedEventPublisherBasePtr;
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_FORWARD_H_11_30_2007

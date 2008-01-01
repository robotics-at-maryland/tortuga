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

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace core {

struct Event;
typedef boost::shared_ptr<Event> EventPtr;

class EventConnection;
typedef boost::shared_ptr<EventConnection> EventConnectionPtr;

class EventHub;
typedef boost::shared_ptr<EventHub> EventHubPtr;

class QueuedEventHub;
typedef boost::shared_ptr<QueuedEventHub> QueuedEventHubPtr;

class QueuedEventHubImp;
typedef boost::shared_ptr<QueuedEventHubImp> QueuedEventHubImpPtr;
    
class EventPublisher;
    
class EventPublisherBase; 
typedef boost::shared_ptr<EventPublisherBase> EventPublisherBasePtr;

class QueuedEventPublisherBase; 
typedef boost::shared_ptr<QueuedEventPublisherBase> QueuedEventPublisherBasePtr;
    
} // namespace core
} // namespace ram
    
// Magic namespace to clean up names
namespace pyplusplus { 
namespace aliases {

typedef ram::core::EventPtr EventPtr;
typedef ram::core::EventConnectionPtr EventConnectionPtr;
typedef ram::core::EventPublisherBasePtr EventPublisherBasePtr;
typedef ram::core::QueuedEventPublisherBasePtr QueuedEventPublisherBasePtr;

}
}

// So GCCXML see's our typedef
namespace details {
inline int instantiateForward()
{
    int a = sizeof(ram::core::EventPtr);
    a += sizeof(ram::core::EventConnectionPtr);
    a += sizeof(ram::core::EventPublisherBasePtr);
    a += sizeof(ram::core::QueuedEventPublisherBasePtr);
    return a;
}
}
    
#endif // RAM_CORE_FORWARD_H_11_30_2007

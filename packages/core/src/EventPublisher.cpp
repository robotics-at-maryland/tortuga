/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/EventPublisher.cpp
 */

// Project Includes
#include "core/include/EventPublisher.h"
#include "core/include/EventPublisherBase.h"

namespace ram {
namespace core {

typedef EventPublisherBaseTemplate<Event::EventType> EventPublisherType;
    
static EventPublisherType*
asType(EventPublisherBasePtr basePtr)
{
    EventPublisherBase* base = basePtr.get();
    EventPublisherType* type =
        dynamic_cast<EventPublisherType*>(base);
    assert(type != 0 && "EventPublisher not of proper type");
    return type; 
}
    
EventPublisher::EventPublisher(EventHubPtr eventHub) :
    m_imp(new EventPublisherBaseTemplate<Event::EventType>(eventHub))
{
}

EventConnectionPtr EventPublisher::subscribe(
    Event::EventType type,
    boost::function<void (EventPtr)> handler)
{
    return asType(m_imp)->subscribe(type, handler);
}

void EventPublisher::publish(Event::EventType type, EventPtr event)
{
    asType(m_imp)->publish(type, type, this, event);
}
/*
void EventPublisher::doPublish(Event::EventType type, EventPublisher* sender,
                               EventPtr event)
{
    asType(m_imp)->publish(type, sender, event);
    }*/

} // namespace core
} // namespace ram


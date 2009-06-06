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
#include "core/include/EventPublisherRegistry.h"

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
    
EventPublisher::EventPublisher(EventHubPtr eventHub, std::string name) :
    m_imp(new EventPublisherBaseTemplate<Event::EventType>(eventHub, name))
{
    if (name != "UNNAMED")
        EventPublisherRegistry::registerPublisher(this);
}

EventPublisher::~EventPublisher()
{
    if (getPublisherName() != "UNNAMED")
        EventPublisherRegistry::unRegisterPublisher(this);
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

std::string EventPublisher::getPublisherName()
{
    return asType(m_imp)->getPublisherName();
}
    
EventPublisher* EventPublisher::lookupByName(std::string name)
{
    return EventPublisherRegistry::lookupByName(name);
}
    
} // namespace core
} // namespace ram


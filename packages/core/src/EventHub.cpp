/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/EventHub.cpp
 */

// Project Includes
#include "core/include/EventHub.h"
#include "core/include/EventPublisherBase.h"
#include "core/include/SubsystemMaker.h"

// Event Types
RAM_CORE_EVENT_TYPE(ram::core::EventHub, ALL_EVENTS);

// Register EventHub into the maker subsystem
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::core::EventHub, EventHub);

namespace ram {
namespace core {

typedef EventPublisherBaseTemplate<Event::EventType> TypeEventPublisherType;
typedef std::pair<Event::EventType, EventPublisher*> TypePublisherPair;
typedef EventPublisherBaseTemplate<TypePublisherPair>
    TypePublisherEventPublisherType;
    

template<class T>
static T*
asType(EventPublisherBasePtr basePtr)
{
    EventPublisherBase* base = basePtr.get();
    T* type =
        dynamic_cast<T*>(base);
    assert(type != 0 && "EventHub EventPublisher not of proper type");
    return type; 
}

EventHub::EventHub(std::string name) : 
    Subsystem(name),
    m_impType(new EventPublisherBaseTemplate<Event::EventType>()),
    m_impTypePublisher(new EventPublisherBaseTemplate<TypePublisherPair>()),
    m_impAll(new EventPublisherBaseTemplate<Event::EventType>())
{
}
    
EventHub::EventHub(ConfigNode config, SubsystemList deps) :
    Subsystem(config["name"].asString()),
    m_impType(new EventPublisherBaseTemplate<Event::EventType>()),
    m_impTypePublisher(new EventPublisherBaseTemplate<TypePublisherPair>()),
    m_impAll(new EventPublisherBaseTemplate<Event::EventType>())
{
}

EventConnectionPtr EventHub::subscribe(
    Event::EventType type,
    EventPublisher* publisher,
    boost::function<void (EventPtr)> handler)
{
    // Subscribe to the internal event publisher which handles subscribes
    // who want events of a particular type from a certain publisher
    TypePublisherPair pair(type, publisher);
    return asType<TypePublisherEventPublisherType>(m_impTypePublisher)->
        subscribe(pair, handler);
}

EventConnectionPtr EventHub::subscribeToType(
    Event::EventType type,
    boost::function<void (EventPtr)> handler)
{
    // Subscribe to the internal event publisher which handles subscribes
    // to a specific event type
    return asType<TypeEventPublisherType>(m_impType)->subscribe(type, handler);
}

    
EventConnectionPtr EventHub::subscribeToAll(
    boost::function<void (EventPtr)> handler)
{
    return asType<TypeEventPublisherType>(m_impAll)->subscribe(
        EventHub::ALL_EVENTS,
        handler);
}
    
void EventHub::publish(EventPtr event)
{
    // Publish to all subscribers of a specific event type
    asType<TypeEventPublisherType>(m_impType)->publish(event->type,
                                                       event->type,
                                                       event->sender,
                                                       event);

    // Publish to all subscribers to specific EventType & EventPublisher pairs
    asType<TypePublisherEventPublisherType>(m_impTypePublisher)->publish(
        std::make_pair(event->type, event->sender),
        event->type,
        event->sender,
        event);

    // Publish to subscribers who want all events
    asType<TypeEventPublisherType>(m_impAll)->publish(
        EventHub::ALL_EVENTS,
        event->type,
        event->sender,
        event);
}

void EventHub::update(double)
{
}
    
void EventHub::background(int)
{
}

void EventHub::unbackground(bool)
{
}

bool EventHub::backgrounded()
{
    return false;
}
    
} // namespace core
} // namespace ram


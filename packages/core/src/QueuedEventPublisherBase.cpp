/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/QueuedEventPublisherBase.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/QueuedEventPublisherBase.h"
#include "core/include/EventPublisher.h"

namespace ram {
namespace core {

QueuedEventPublisherBase::Connection::Connection(EventConnectionPtr internal,
                                                 QueuedEventPublisherBase* publisher) :
    m_internal(internal),
    m_publisher(publisher)
{
}
    
Event::EventType QueuedEventPublisherBase::Connection::getType()
{
    return m_internal->getType();
}
    
void QueuedEventPublisherBase::Connection::disconnect()
{
    // Undo external one first
    m_publisher->unSubscribe(m_internal->getType());

    // Now the internal
    m_internal->disconnect();
}
    
    

QueuedEventPublisherBase::QueuedEventPublisherBase(EventPublisher* parent) :
    m_parent(parent)
{
}
    
EventConnectionPtr QueuedEventPublisherBase::subscribe(
    Event::EventType type, 
    boost::function<void (EventPtr)>  handler)
{
    ReadWriteMutex::ScopedWriteLock setLock(m_connectionsMutex);

    EventConnectionPtr externalConnection;
    ConnectionsMapIter iter = m_connectionTypes.find(type);    
    if (m_connectionTypes.end() == iter)
    {
        // Subscribe to parent
        externalConnection =
            m_parent->subscribe(type,
                                boost::bind(&QueuedEventPublisherBase::queueEvent,
                                            this, _1));
        m_connectionTypes[type] = std::make_pair(1, externalConnection);
    }
    else
    {
        int refCount;
        boost::tie(refCount, externalConnection) = iter->second;
        iter->second = std::make_pair(refCount + 1, externalConnection);
    }

    // Subscribe to the internal publisher
    EventConnectionPtr internalConnection(EventPublisherBase::subscribe(type,
                                                                        handler));
    return EventConnectionPtr(
        new QueuedEventPublisherBase::Connection(internalConnection,
                                             this));
}
    
void QueuedEventPublisherBase::publish(Event::EventType type, EventPublisher* sender,
                                       EventPtr event)
{
    // Queue event internally
    event->type = type;
    event->sender = sender;
    queueEvent(event);
};

int QueuedEventPublisherBase::publishEvents()
{
    EventPtr event;
    int published = 0;
    while(m_eventQueue.popNoWait(event))
    {
        EventPublisherBase::publish(event->type, event->sender, event);
        published++;
    }
    return published;
}

int QueuedEventPublisherBase::waitAndPublishEvents()
{
    // Wait for events and publish the new event
    EventPtr event = m_eventQueue.popWait();
    EventPublisherBase::publish(event->type, event->sender, event);
    
    return 1 + publishEvents();
}
    
void QueuedEventPublisherBase::unSubscribe(Event::EventType type)
{
    ReadWriteMutex::ScopedWriteLock setLock(m_connectionsMutex);

    // Decrement number of connections of a this type
    ConnectionsMapIter iter = m_connectionTypes.find(type);
    assert(m_connectionTypes.end() != iter && "Invalid connection");

    int refCount;
    EventConnectionPtr connection;
    // Grab current refCount and EventConnectionPtr
    boost::tie(refCount, connection) = iter->second;

    if (1 == refCount)
    {
        m_connectionTypes.erase(iter);
        connection->disconnect();
    }
    else
    {
        // Decrement the refCount
        iter->second = std::make_pair(refCount - 1, connection);
    }
}
    
void QueuedEventPublisherBase::queueEvent(EventPtr event)
{
    m_eventQueue.push(event);
}
    
} // namespace core
} // namespace ram

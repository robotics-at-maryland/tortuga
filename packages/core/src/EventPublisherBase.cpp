/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventPublisherBase.h
 */

// Project Includes
#include "core/include/EventPublisherBase.h"

namespace ram {
namespace core {

EventPublisherBase::Connection::Connection(Event::EventType type,
                                           EventPublisherBase* publisher,
                                       boost::signals::connection connection) :
    m_type(type),
    m_publisher(publisher),
    m_connection(connection)
{
}

Event::EventType EventPublisherBase::Connection::getType()
{
    return m_type;
}
    
void EventPublisherBase::Connection::disconnect()
{
    m_publisher->unSubscribe(m_type, m_connection);
}
    
EventConnectionPtr EventPublisherBase::subscribe(
        Event::EventType type,
        boost::function<void (EventPtr)> handler)
{
    ReadWriteMutex::ScopedWriteLock lock(m_signalMapMutex);
    return EventConnectionPtr(
        new EventPublisherBase::Connection(type, this,
                                           m_signals[type].connect(handler)));
}

void EventPublisherBase::publish(Event::EventType type, 
                                 EventPublisher* sender,
                                 EventPtr event)
{
    // Set event property
    event->type = type;
    event->sender = sender;

    {
        ReadWriteMutex::ScopedReadLock mapLock(m_signalMapMutex);
        boost::mutex::scoped_lock lock(m_signalMutexes[type]);

        // Call subscribers
        m_signals[type](event);
    }
}

void EventPublisherBase::unSubscribe(Event::EventType type,
                                     boost::signals::connection connection)
{
    ReadWriteMutex::ScopedWriteLock mapLock(m_signalMapMutex);
    boost::mutex::scoped_lock lock(m_signalMutexes[type]);
    connection.disconnect();
}
    
} // namespace core
} // namespace ram


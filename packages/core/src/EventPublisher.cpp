/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventPublisher.h
 */

// Project Includes
#include "core/include/EventPublisher.h"

namespace ram {
namespace core {

EventPublisher::Connection::Connection(Event::EventType type,
                                       EventPublisher* publisher,
                                       boost::signals::connection connection) :
    m_type(type),
    m_publisher(publisher),
    m_connection(connection)
{
}

Event::EventType EventPublisher::Connection::getType()
{
    return m_type;
}
    
void EventPublisher::Connection::disconnect()
{
    m_publisher->unSubscribe(m_type, m_connection);
}
    
EventConnectionPtr EventPublisher::subscribe(Event::EventType type,
                                                     EventSlot handler)
{
    ReadWriteMutex::ScopedWriteLock lock(m_signalMapMutex);
    return EventConnectionPtr(
        new EventPublisher::Connection(type, this,
                                       m_signals[type].connect(handler)));
}
    
void EventPublisher::publish(Event::EventType type, EventPtr event)
{
    doPublish(type, this, event);
}

void EventPublisher::doPublish(Event::EventType type, EventPublisher* sender,
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

void EventPublisher::unSubscribe(Event::EventType type,
                                 boost::signals::connection connection)
{
    ReadWriteMutex::ScopedWriteLock mapLock(m_signalMapMutex);
    boost::mutex::scoped_lock lock(m_signalMutexes[type]);
    connection.disconnect();
}
    
} // namespace core
} // namespace ram


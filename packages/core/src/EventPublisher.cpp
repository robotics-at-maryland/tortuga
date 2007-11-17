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

void EventPublisher::subscribe(Event::EventType type, EventSlot handler)
{
    ReadWriteMutex::ScopedWriteLock lock(m_signalMapMutex);
    m_signals[type].connect(handler);
}

void EventPublisher::publish(Event::EventType type, EventPtr event)
{
    // Set event property
    event->type = type;
    event->sender = this;

    {
        ReadWriteMutex::ScopedReadLock mapLock(m_signalMapMutex);
        boost::mutex::scoped_lock lock(m_signalMutexes[type]);

        // Call subscribers
        m_signals[type](event);
    }
}
    
} // namespace core
} // namespace ram


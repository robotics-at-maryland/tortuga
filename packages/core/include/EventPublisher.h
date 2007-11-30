/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventPublisher.h
 */

#ifndef RAM_CORE_EVENTPUBLISHER_H_11_19_2007
#define RAM_CORE_EVENTPUBLISHER_H_11_19_2007

// Library Includes
#include <boost/signal.hpp>
#include <boost/ptr_container/ptr_map.hpp>

// Project Includes
#include "core/include/ReadWriteMutex.h"
#include "core/include/Event.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

typedef boost::signal<void (EventPtr)>  EventSignal;
typedef EventSignal::slot_type EventSlot;
    
class RAM_EXPORT EventPublisher
{
public:
    void subscribe(Event::EventType type, EventSlot handler);
    void publish(Event::EventType type, EventPtr event);
    
private:
    /// Protects access to map of types->signals
    ReadWriteMutex m_signalMapMutex;

    /// Protects access to each signal (they are not thread safe)
    boost::ptr_map<Event::EventType, boost::mutex> m_signalMutexes;

    /// Maps types -> signals
    boost::ptr_map<Event::EventType, EventSignal>  m_signals;
};
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENTPUBLISHER_H_11_19_2007

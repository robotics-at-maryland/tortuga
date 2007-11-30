/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventPublisherBase.h
 */

#ifndef RAM_CORE_EVENTPUBLISHERBASE_H_11_30_2007
#define RAM_CORE_EVENTPUBLISHERBASE_H_11_30_2007

// Library Includes
#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/ptr_container/ptr_map.hpp>

// Project Includes
#include "core/include/ReadWriteMutex.h"
#include "core/include/EventConnection.h"
#include "core/include/Forward.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {
    
class EventPublisherBase
{
public:
    virtual ~EventPublisherBase() {};

    virtual EventConnectionPtr subscribe(
        Event::EventType type,
        boost::function<void (EventPtr)> handler);
    
    virtual void publish(Event::EventType type, EventPublisher* sender,
                         EventPtr event);
    
private:
    /** Remove handler from recieving particular event types */
    void unSubscribe(Event::EventType type,
                     boost::signals::connection connection);

    /// Implements the abstract connection class
    class Connection : public EventConnection
    {
    public:
        Connection(Event::EventType type,
                   EventPublisherBase* publisher,
                   boost::signals::connection connection);
    
        virtual Event::EventType getType();
        
        virtual void disconnect();
    private:
        /** Type of the event */
        Event::EventType m_type;
        
        /** Publisher to which the event is connection */
        EventPublisherBase* m_publisher;

        /** The underlying boost connection */
        boost::signals::connection m_connection;
    };

    // So it can call unSubscribe
    friend class Connection;
    
    /// Protects access to map of types->signals
    ReadWriteMutex m_signalMapMutex;

    /// Protects access to each signal (they are not thread safe)
    boost::ptr_map<Event::EventType, boost::mutex> m_signalMutexes;

    typedef boost::signal<void (EventPtr)>  EventSignal;
    /// Maps types -> signals
    boost::ptr_map<Event::EventType, EventSignal>  m_signals;
};
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENTPUBLISHERBASE_H_11_30_2007

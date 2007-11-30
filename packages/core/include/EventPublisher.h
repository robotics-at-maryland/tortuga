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
#include <boost/shared_ptr.hpp>
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

class EventConnection : boost::noncopyable
{
public:
    virtual ~EventConnection() {}
    virtual Event::EventType getType() = 0;
    virtual void disconnect() = 0;
protected:
    EventConnection() {}
};

typedef boost::shared_ptr<EventConnection> EventConnectionPtr;
    
class RAM_EXPORT EventPublisher
{
public:
    virtual ~EventPublisher() {};

    /** Subscribe to recieve events of the given type
     *
     *  The given function will be called any every time an event of the
     *  registered type is publihsed.
     *
     *  @param type     The kind of event to listen for
     *  @param handler  A void(EventPtr) function object to recieve the event
     *  @return         The connection object, needed for disconnection.
     */
    virtual EventConnectionPtr subscribe(Event::EventType type,
                                         EventSlot handler);
    
    /** Call all handlers of the given type with the given event */
    virtual void publish(Event::EventType type, EventPtr event);

protected:
    void doPublish(Event::EventType type, EventPublisher* sender,
                   EventPtr event);
    
    /** Remove handler from recieving particular event types */
    void unSubscribe(Event::EventType type,
                     boost::signals::connection connection);
    
private:
    /// Implements the abstract connection class
    class Connection : public EventConnection
    {
    public:
        virtual Event::EventType getType();
        
        virtual void disconnect();

    protected:
        Connection(Event::EventType type,
                   EventPublisher* publisher,
                   boost::signals::connection connection);

    private:
        // So it can be constructed
        friend class EventPublisher;

        /** Type of the event */
        Event::EventType m_type;
        
        /** Publisher to which the event is connection */
        EventPublisher* m_publisher;

        /** The underlying boost connection */
        boost::signals::connection m_connection;
    };

    // So it can call unSubscribe
    friend class Connection;
    
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

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
    virtual ~EventPublisherBase() {}
};

template<typename T>
class EventPublisherBaseTemplate :
    public EventPublisherBase
{
public:
    virtual ~EventPublisherBaseTemplate() {};

    virtual EventConnectionPtr subscribe(
        T type,
        boost::function<void (EventPtr)> handler);
    
    virtual void publish(T type, EventPublisher* sender,
                         EventPtr event);

private:
    /** Remove handler from recieving particular event types */
    void unSubscribe(T type,
                     boost::signals::connection connection);

    /// Implements the abstract connection class
    class Connection : public EventConnection
    {
    public:
        Connection(T type,
                   EventPublisherBaseTemplate<T>* publisher,
                   boost::signals::connection connection);
    
        virtual T getType();
        
        virtual void disconnect();
    private:
        /** Type of the event */
        T m_type;
        
        /** Publisher to which the event is connection */
        EventPublisherBaseTemplate* m_publisher;

        /** The underlying boost connection */
        boost::signals::connection m_connection;
    };

    // So it can call unSubscribe
    friend class Connection;
    
    /// Protects access to map of types->signals
    ReadWriteMutex m_signalMapMutex;

    /// Protects access to each signal (they are not thread safe)
    boost::ptr_map<T, boost::mutex> m_signalMutexes;

    typedef boost::signal<void (EventPtr)>  EventSignal;
    /// Maps types -> signals
    boost::ptr_map<T, EventSignal>  m_signals;
};

// Implementation of the template functions
    
template<typename T>
EventConnectionPtr EventPublisherBaseTemplate<T>::subscribe(
    T type,
    boost::function<void (EventPtr)> handler)
{
    ReadWriteMutex::ScopedWriteLock lock(m_signalMapMutex);
    return EventConnectionPtr(
        new typename EventPublisherBaseTemplate<T>::Connection(type, this,
            m_signals[type].connect(handler)));
}

template<typename T>
void EventPublisherBaseTemplate<T>::publish(T type, EventPublisher* sender,
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

template<typename T>
void EventPublisherBaseTemplate<T>::unSubscribe(T type,
    boost::signals::connection connection)
{
    ReadWriteMutex::ScopedWriteLock mapLock(m_signalMapMutex);
    boost::mutex::scoped_lock lock(m_signalMutexes[type]);
    connection.disconnect();
}

template<typename T>
EventPublisherBaseTemplate<T>::Connection::Connection(T type,
                   EventPublisherBaseTemplate<T>* publisher,
                   boost::signals::connection connection) :
    m_type(type),
    m_publisher(publisher),
    m_connection(connection)
{
}

template<typename T>
T EventPublisherBaseTemplate<T>::Connection::getType()
{
    return m_type;
}

template<typename T>
void EventPublisherBaseTemplate<T>::Connection::disconnect()
{
    m_publisher->unSubscribe(m_type, m_connection);
}

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENTPUBLISHERBASE_H_11_30_2007

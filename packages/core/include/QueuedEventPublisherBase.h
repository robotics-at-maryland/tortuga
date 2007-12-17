/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/QueuedEventPublisherBase.h
 */

#ifndef RAM_CORE_QUEUEDEVENTPUBLISHERBASE_H_11_30_2007
#define RAM_CORE_QUEUEDEVENTPUBLISHERBASE_H_11_30_2007

// STD Includes
#include <map>

// Library Includes
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/EventPublisherBase.h"
#include "core/include/EventConnection.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/ThreadedQueue.h"

namespace ram {
namespace core {

class QueuedEventPublisherBase
{
public:
    virtual ~QueuedEventPublisherBase() {}
};

template<typename T>
class QueuedEventPublisherBaseTemplate :
        public QueuedEventPublisherBase,
        public EventPublisherBaseTemplate<T>
{
public:
    QueuedEventPublisherBaseTemplate(EventPublisher* parent);

    virtual EventConnectionPtr subscribe(T type,
        boost::function<void (EventPtr)>  handler);
    
    virtual void publish(T type, EventPublisher* sender,
                         EventPtr event);

    int publishEvents();
    
    int waitAndPublishEvents();
    
private:
    typedef std::pair<int, EventConnectionPtr> EventRecord;
    typedef std::map<T, EventRecord> ConnectionsMap;
    typedef typename ConnectionsMap::iterator ConnectionsMapIter;
    
    class Connection : public EventConnection
    {
    public:
        Connection(EventConnectionPtr internal,
                   QueuedEventPublisherBaseTemplate<T>* publisher);
    
        virtual T getType();
        virtual void disconnect();
        
    private:

        EventConnectionPtr m_internal;
        QueuedEventPublisherBaseTemplate* m_publisher;
    };
    friend class Connection;

    void unSubscribe(T type);
    
    /** Places all recieved events on the internal queue */
    void queueEvent(EventPtr event);

    /** The source of events we are queueing */
    EventPublisher* m_parent;

    /** Thread safe queue for events */
    ThreadedQueue<EventPtr> m_eventQueue;

    /** Protects access to map of types->signals */
    ReadWriteMutex m_connectionsMutex;
    
    /** Makes sure we only subscribe once for each event type */
    ConnectionsMap m_connectionTypes;
};

// Template Implementation
template<typename T>
QueuedEventPublisherBaseTemplate<T>::Connection::Connection(
    EventConnectionPtr internal,
    QueuedEventPublisherBaseTemplate<T>* publisher) :
    m_internal(internal),
    m_publisher(publisher)
{
}

template<typename T>
T QueuedEventPublisherBaseTemplate<T>::Connection::getType()
{
    return m_internal->getType();
}

template<typename T>
void QueuedEventPublisherBaseTemplate<T>::Connection::disconnect()
{
    // Undo external one first
    m_publisher->unSubscribe(m_internal->getType());

    // Now the internal
    m_internal->disconnect();
}


template<typename T>
QueuedEventPublisherBaseTemplate<T>::QueuedEventPublisherBaseTemplate(
    EventPublisher* parent) :
    m_parent(parent)
{
}

template<typename T>
EventConnectionPtr QueuedEventPublisherBaseTemplate<T>::subscribe(
    T type, 
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
                boost::bind(&QueuedEventPublisherBaseTemplate<T>::queueEvent,
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
    EventConnectionPtr internalConnection(
        EventPublisherBaseTemplate<T>::subscribe(type,
                                                 handler));
    return EventConnectionPtr(
        new typename QueuedEventPublisherBaseTemplate<T>::Connection(
            internalConnection,
            this));
}

template<typename T>
void QueuedEventPublisherBaseTemplate<T>::publish(T type,
                                                  EventPublisher* sender,
                                                  EventPtr event)
{
    // Queue event internally
    event->type = type;
    event->sender = sender;
    queueEvent(event);
};

template<typename T>
int QueuedEventPublisherBaseTemplate<T>::publishEvents()
{
    EventPtr event;
    int published = 0;
    while(m_eventQueue.popNoWait(event))
    {
        EventPublisherBaseTemplate<T>::publish(event->type,
                                               event->sender,
                                               event);
        published++;
    }
    return published;
}

template<typename T>
int QueuedEventPublisherBaseTemplate<T>::waitAndPublishEvents()
{
    // Wait for events and publish the new event
    EventPtr event = m_eventQueue.popWait();
    EventPublisherBaseTemplate<T>::publish(event->type,
                                           event->sender,
                                           event);
    
    return 1 + publishEvents();
}

template<typename T>
void QueuedEventPublisherBaseTemplate<T>::unSubscribe(T type)
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

template<typename T>
void QueuedEventPublisherBaseTemplate<T>::queueEvent(EventPtr event)
{
    m_eventQueue.push(event);
}

} // namespace core
} // namespace ram

#endif // RAM_CORE_QUEUEDEVENTPUBLISHERBASE_H_11_30_2007

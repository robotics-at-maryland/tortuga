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

// Project Includes
#include "core/include/EventPublisherBase.h"
#include "core/include/EventConnection.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/ThreadedQueue.h"

namespace ram {
namespace core {

class QueuedEventPublisherBase :
        public EventPublisherBaseTemplate<Event::EventType>
{
public:
    QueuedEventPublisherBase(EventPublisher* parent);

    virtual EventConnectionPtr subscribe(Event::EventType type,
                                         boost::function<void (EventPtr)>  handler);
    
    virtual void publish(Event::EventType type, EventPublisher* sender,
                         EventPtr event);

    int publishEvents();
    
    int waitAndPublishEvents();
    
private:
    typedef std::pair<int, EventConnectionPtr> EventRecord;
    typedef std::map<Event::EventType, EventRecord> ConnectionsMap;
    typedef ConnectionsMap::iterator ConnectionsMapIter;
    
    class Connection : public EventConnection
    {
    public:
        Connection(EventConnectionPtr internal,
                   QueuedEventPublisherBase* publisher);
    
        virtual Event::EventType getType();
        virtual void disconnect();
        
    private:

        EventConnectionPtr m_internal;
        QueuedEventPublisherBase* m_publisher;
    };
    friend class Connection;

    void unSubscribe(Event::EventType type);
    
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

} // namespace core
} // namespace ram

#endif // RAM_CORE_QUEUEDEVENTPUBLISHERBASE_H_11_30_2007

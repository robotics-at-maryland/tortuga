/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/QueuedEventPublisher.h
 */

#ifndef RAM_CORE_QUEUEDEVENTPUBLISHER_H_11_24_2007
#define RAM_CORE_QUEUEDEVENTPUBLISHER_H_11_24_2007

// STD Includes
#include <map>

// Project Includes
#include "core/include/EventPublisher.h"
#include "core/include/ThreadedQueue.h"

namespace ram {
namespace core {

class QueuedEventPublisher : public EventPublisher
{
public:
    /** Create a QueuedEventPublisher connected to the given EventPublisher
     *
     *  This provides a buffer inbetween a handler function and EventPublishers.
     *  All event subscribtions are forward to the given EventPublishers.  Then
     *  all incoming events are queued up, until they are released.
     *
     *  @param parent The EventPublisher to subcribe to, and handle events from.
     */
    QueuedEventPublisher(EventPublisher* parent);

    /** Subscribe to recieve events of the given type
     *
     *  This will cause the QueuedEventPublisher to subscribe to its given
     *  EventPublishers for that same event and record all recieved events.
     *  When processEvents is called those events will be delived to subscribed
     *  handler.
     *
     *  @param type     The kind of event to listen for
     *  @param handler  A void(EventPtr) function object to recieve the event
     *  @return         The connection object, needed for disconnection.
     */
    virtual EventConnectionPtr subscribe(Event::EventType type,
                                         EventSlot handler);
    
    /** Publish the event to the internal queue */
    virtual void publish(Event::EventType type, EventPtr event);

    /** Publishes all queued events and any that arrive while publishing those events
        @return The number of events published
     */
    int publishEvents();


    /** Waits for the next event, then publishs all queued Events
        @return The number of events published
    */
    int waitAndPublishEvents();
protected:

    
private:
    typedef std::pair<int, EventConnectionPtr> EventRecord;
    typedef std::map<Event::EventType, EventRecord> ConnectionsMap;
    typedef ConnectionsMap::iterator ConnectionsMapIter;
    
    class Connection : public EventConnection
    {
    public:
        virtual Event::EventType getType();
        virtual void disconnect();
        
    private:
        // So it can be constructed
        friend class QueuedEventPublisher;
        
        Connection(EventConnectionPtr internal,
                   QueuedEventPublisher* publisher);

        EventConnectionPtr m_internal;
        QueuedEventPublisher* m_publisher;
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

#endif // RAM_CORE_QUEUEDEVENTPUBLISHER_H_11_24_2007

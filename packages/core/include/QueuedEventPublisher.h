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

// Project Includes
#include "core/include/EventPublisher.h"
#include "core/include/Forward.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class RAM_EXPORT QueuedEventPublisher : public EventPublisher // noncopyable
{
public:
    /** Create a QueuedEventPublisher connected to the given EventPublisher
     *
     *  This provides a buffer inbetween a handler function and EventPublishers.
     *  All event subscribtions are forwarded to the given EventPublishers.
     *  Then all incoming events are queued up, until they are released.
     *
     *  @param parent
     *      The EventPublisher to subcribe to, and handle events from.
     */
    QueuedEventPublisher(EventPublisher* parent);

    /** Subscribe to recieve events of the given type
     *
     *  This will cause the QueuedEventPublisher to subscribe to its given
     *  EventPublishers for that same event and record all recieved events.
     *  When processEvents is called those events will be delived to subscribed
     *  handler.
     *
     *  @param type
     *      The kind of event to listen for
     *  @param handler
     *      A void(EventPtr) function object to recieve the event
     *  @return
     *      The connection object, needed for disconnection.
     */
    virtual EventConnectionPtr subscribe(Event::EventType type,
                                         boost::function<void (EventPtr)>  handler);
    
    /**Publish the event to the internal queue */
    virtual void publish(Event::EventType type, EventPtr event);

    /** Publishes all queued events and any that arrive while publishing those events
     *
     *  @return
     *      The number of events published
     */
    int publishEvents();
    
    /** Waits for the next event, then publishs all queued Events
     *
     *  @return
     *      The number of events published
     */
    int waitAndPublishEvents();
    
private:
    QueuedEventPublisherBasePtr m_imp;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_QUEUEDEVENTPUBLISHER_H_11_24_2007

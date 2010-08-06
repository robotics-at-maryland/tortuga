/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventHub.h
 */

#ifndef RAM_CORE_EVENTHUB_12_17_2007
#define RAM_CORE_EVENTHUB_12_17_2007

// Library Includes
#include <boost/utility.hpp>
#include <boost/function.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

/** A hub for events from EventPublisher objects
 *
 *  This is an object which, when passed to EventPublisher, also recieves all
 *  events published through that EventPublisher.  You can pass the same hub to
 *  multiple EventPublishers and be able to easy subscribe to the events that
 *  group of EventPublishers creates.
 *
 *  This object itself is an EventPublisher and its events are published the
 *  hub as well.  It currently does not generate any events.
 */
class RAM_EXPORT EventHub : public Subsystem
{
public:
    static const ram::core::Event::EventType ALL_EVENTS;
    
    /** Easier to use contructor */
    EventHub(std::string name = "EventHub");

    /** Standard subsystem constructor */
    EventHub(ConfigNode config, SubsystemList deps = SubsystemList());

    virtual ~EventHub();

    /** Subscribe to recieve events of the given type from a certain Publisher
     *
     *  The given function will be called any every time an event of the given
     *  type, from the given EventPublisher is published to this hub.
     *
     *  @param type      The kind of event to listen for
     *  @param publisher The event publisher you wish to recieve events from
     *  @param handler   A void(EventPtr) function object to recieve the event
     *
     *  @return         The connection object, needed for disconnection.
     */
    virtual EventConnectionPtr subscribe(
        Event::EventType type,
        EventPublisher* publisher,
        boost::function<void (EventPtr)> handler);
    
    /** Subscribe to recieve events of the given type from all publishers
     *
     *  The given function will be called any every time an event of the
     *  given type is publihsed, regardless of what EventPublisher published
     *  it.
     *
     *  @param type     The kind of event to listen for
     *  @param handler  A void(EventPtr) function object to recieve the event
     *
     *  @return         The connection object, needed for disconnection.
     */
    virtual EventConnectionPtr subscribeToType(
        Event::EventType type,
        boost::function<void (EventPtr)> handler);

    /** Subscribe to recieve all events published to this EventHub
     *
     *  @param handler   A void(EventPtr) function object to recieve the event
     *
     *  @return         The connection object, needed for disconnection.
     */
    virtual EventConnectionPtr subscribeToAll(
        boost::function<void (EventPtr)> handler);

    using EventPublisher::publish;
    
    /** Publishes and event to the hub
     *
     *  All fields in the event must be fully filled out and not in an
     *  undefined state.
     *
     *  @param event A shared_ptr to the Event object that is to be published.
     */
    virtual void publish(EventPtr event);

    /** Publishs an event of the given type as the event hub
     *
     *  This event goes out through the event hub as well.
     */
    virtual void publish(Event::EventType type, EventPtr event);

    /** Does nothing for this class */
    virtual void update(double timestep);
    /** Does nothing for this class */
    virtual void setPriority(IUpdatable::Priority priority);
    /** Does nothing for this class, always returns NORMAL_PRIORITY */
    virtual IUpdatable::Priority getPriority();
    /** Does nothing for this class */
    virtual void setAffinity(size_t affinity);
    /** Does nothing for this class, always returns -1 */
    virtual int getAffinity();
    /** Does nothing for this class */
    virtual void background(int interval);
    /** Does nothing for this class */
    virtual void unbackground(bool join);
    /** Does nothing for this class, always returns false  */
    virtual bool backgrounded();
    
private:
    /// The Publisher used for EventType subscribers
    EventPublisherBasePtr m_impType;

    /// The Publisher used for EventType & EventPublisher subscribers
    EventPublisherBasePtr m_impTypePublisher;

    /// The Publisher used for subscribers to all messages
    EventPublisherBasePtr m_impAll;
};
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENTHUB_12_17_2007

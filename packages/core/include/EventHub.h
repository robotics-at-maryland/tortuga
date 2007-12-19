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
    
class RAM_EXPORT EventHub : public Subsystem
{
public:
    /** Easier to use contructor */
    EventHub(std::string name = "EventHub");

    /** Standard subsystem constructor */
    EventHub(ConfigNode config, SubsystemList deps = SubsystemList());

    virtual ~EventHub() {};

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
    
    /** Publishes and event to the hub
     *
     *  All fields in the event must be fully filled out and not in an
     *  undefined state.
     *
     *  @param event A shared_ptr to the Event object that is to be published.
     */
    virtual void publish(EventPtr event);

    virtual void update(double);
    virtual void background(int);
    virtual void unbackground(bool);
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

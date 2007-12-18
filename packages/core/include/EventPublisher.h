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
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

// Project Includes
#include "core/include/Event.h"
#include "core/include/Forward.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {
    
class RAM_EXPORT EventPublisher : public boost::noncopyable
{
public:
    /** Create an EventPublisher
     *
     *  @param eventHub  If non zero, the EventPublisher will publish all
     *      events it recievers there as well as to its own subscribers.
     *
     */
    EventPublisher(EventHubPtr eventHub = EventHubPtr());

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
                                         boost::function<void (EventPtr)> handler);
    
    /** Call all handlers of the given type with the given event */
    virtual void publish(Event::EventType type, EventPtr event);
    
//protected:
    /** Publishes the message with the desired sender */
//    void doPublish(Event::EventType type, EventPublisher* sender,
//                   EventPtr event);
    
private:
    EventPublisherBasePtr m_imp;
};
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENTPUBLISHER_H_11_19_2007

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/QueuedEventHub.h
 */

#ifndef RAM_CORE_QUEUEDEVENTHUB_12_25_2007
#define RAM_CORE_QUEUEDEVENTHUB_12_25_2007

// Library Includes
#include <boost/function.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "core/include/EventHub.h"
#include "core/include/Forward.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

/** An EventHub which queues events until its told to publish them
 *      
 *  This class can be used to isolate the user from having its EventHandlers
 *  being called from other threads. It currently queues all events it recieves
 *  not just the ones that are currently subscribed for.
 */
class RAM_EXPORT QueuedEventHub : public EventHub
{
public:
    /** Normal contructor */
    QueuedEventHub(ram::core::EventHubPtr eventHub,
                   std::string name = "QueuedEventHub");

    /** Standard subsystem constructor */
    QueuedEventHub(ConfigNode config, SubsystemList deps = SubsystemList());

    virtual ~QueuedEventHub();

    /** Places the event on the intneral event queue*/
    virtual void publish(EventPtr event);

    /** Publishs the event into the internal event queue (with sender=this) */
    virtual void publish(Event::EventType type, EventPtr event);
    
    /** @copydoc QueuedEventPublisher::publishEvents() */
    int publishEvents();
    
    /** @copydoc QueuedEventPublisher::waitAndPublishEvents() */
    int waitAndPublishEvents();
    
    /** Has the same effect as publishEvents() */
    virtual void update(double timestep);

    virtual bool backgrounded() { return false; }

private:
    /** Publishes the event to all subscribers */
    void _publish(EventPtr event);

    /** The EventHub we are connected to */
    EventHubPtr m_hub;
    
    /// The implementation of the QueuedEventHub
    QueuedEventHubImpPtr m_imp;

    /// Connection EventHub being queued
    EventConnectionPtr m_connection;
};
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_QUEUEDEVENTHUB_12_25_2007

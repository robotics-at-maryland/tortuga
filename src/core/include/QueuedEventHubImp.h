/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/QueuedEventHubImp.h
 */

#ifndef RAM_CORE_QUEUEDEVENTHUBIMP_12_26_2007
#define RAM_CORE_QUEUEDEVENTHUBIMP_12_26_2007

// Library Includes
#include <boost/function.hpp>

// Project Includes
#include "core/include/ThreadedQueue.h"
#include "core/include/Forward.h"
#include "core/include/Event.h"

namespace ram {
namespace core {

class QueuedEventHubImp
{
public:
    /** Creates a new instance */ 
    QueuedEventHubImp();

    /** Set the function used twhich publishes use the given function */
    void setPublishFunction(boost::function<void (EventPtr)> publishFunction);
    
    /** Store event on the internal queue */
    void queueEvent(EventPtr event);

    /** @copydoc QueuedEventPublisher::publishEvents() */
    int publishEvents();

    /** @copydoc QueuedEventPublisher::waitAndPublishEvents() */
    int waitAndPublishEvents();
    
private:
    /** Function which events are published to */
    boost::function<void (EventPtr)> m_publishFunction;
    
    /** Thread safe queue for events */
    ThreadedQueue<EventPtr> m_eventQueue;
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_QUEUEDEVENTHUBIMP_12_26_2007

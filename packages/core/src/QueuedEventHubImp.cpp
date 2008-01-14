/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/QueuedEventHubImp.cpp
 */

// Project Includes
#include "core/include/QueuedEventHubImp.h"

namespace ram {
namespace core {

QueuedEventHubImp::QueuedEventHubImp()
{
}

void QueuedEventHubImp::setPublishFunction(boost::function<void (EventPtr)> publishFunction)
{
    m_publishFunction = publishFunction;
}
    
void QueuedEventHubImp::queueEvent(EventPtr event)
{
    m_eventQueue.push(event);    
}
                                   
int QueuedEventHubImp::publishEvents()
{
    EventPtr event;
    int published = 0;
    
    while(m_eventQueue.popNoWait(event))
    {
        m_publishFunction(event);
        published++;
    }
    
    return published;    
}

int QueuedEventHubImp::waitAndPublishEvents()
{
    // Wait for events and publish the new event
    EventPtr event = m_eventQueue.popWait();
    
    m_publishFunction(event);
    
    return 1 + publishEvents();    
}
    
} // namespace core
} // namespace ram


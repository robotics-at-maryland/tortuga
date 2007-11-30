/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/QueuedEventPublisher.cpp
 */

// Project Includes
#include "core/include/QueuedEventPublisher.h"
#include "core/include/QueuedEventPublisherBase.h"

namespace ram {
namespace core {

QueuedEventPublisher::QueuedEventPublisher(EventPublisher* parent) :
    m_imp(new QueuedEventPublisherBase(parent))
{
}
    
EventConnectionPtr QueuedEventPublisher::subscribe(
    Event::EventType type, 
    boost::function<void (EventPtr)>  handler)
{
    return m_imp->subscribe(type, handler);
}
    
void QueuedEventPublisher::publish(Event::EventType type, EventPtr event)
{
    m_imp->publish(type, this, event);
};

int QueuedEventPublisher::publishEvents()
{
    return m_imp->publishEvents();
}

int QueuedEventPublisher::waitAndPublishEvents()
{
    return m_imp->waitAndPublishEvents();
}
    
} // namespace core
} // namespace ram

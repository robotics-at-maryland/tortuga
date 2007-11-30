/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventPublisher.h
 */

// Project Includes
#include "core/include/EventPublisher.h"
#include "core/include/EventPublisherBase.h"

namespace ram {
namespace core {

EventPublisher::EventPublisher() :
    m_imp(new EventPublisherBase())
{
}

EventConnectionPtr EventPublisher::subscribe(
    Event::EventType type,
    boost::function<void (EventPtr)> handler)
{
    return m_imp->subscribe(type, handler);
}

void EventPublisher::publish(Event::EventType type, EventPtr event)
{
    m_imp->publish(type, this, event);
}

void EventPublisher::doPublish(Event::EventType type, EventPublisher* sender,
                               EventPtr event)
{
    m_imp->publish(type, sender, event);
}

} // namespace core
} // namespace ram


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

typedef QueuedEventPublisherBaseTemplate<Event::EventType> QueuedPublisherType;
    
static QueuedPublisherType*
asType(QueuedEventPublisherBasePtr basePtr)
{
    QueuedEventPublisherBase* base = basePtr.get();
    QueuedPublisherType* type =
        dynamic_cast<QueuedPublisherType*>(base);
    assert(type != 0 && "QueuedEventPublisher not of proper type");
    return type; 
}
    
QueuedEventPublisher::QueuedEventPublisher(EventPublisher* parent) :
    m_imp(new QueuedEventPublisherBaseTemplate<Event::EventType>(parent,
                                                                 "UNNAMED"))
{
}
    
EventConnectionPtr QueuedEventPublisher::subscribe(
    Event::EventType type, 
    boost::function<void (EventPtr)>  handler)
{
    return asType(m_imp)->subscribe(type, handler);
}
    
void QueuedEventPublisher::publish(Event::EventType type, EventPtr event)
{
    asType(m_imp)->publish(type, type, this, event);
};

int QueuedEventPublisher::publishEvents()
{
    return asType(m_imp)->publishEvents();
}

int QueuedEventPublisher::waitAndPublishEvents()
{
    return asType(m_imp)->waitAndPublishEvents();
}
    
} // namespace core
} // namespace ram

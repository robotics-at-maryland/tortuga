/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/QueuedEventHub.cpp
 */


// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "core/include/QueuedEventHub.h"
#include "core/include/QueuedEventHubImp.h"
#include "core/include/EventConnection.h"
#include "core/include/SubsystemMaker.h"

// Register EventHub into the maker subsystem
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::core::QueuedEventHub, QueuedEventHub);

namespace ram {
namespace core {

QueuedEventHub::QueuedEventHub(ram::core::EventHubPtr eventHub,
                               std::string name) :
    EventHub(name),
    m_hub(eventHub),
    m_imp(new QueuedEventHubImp()),
    // Send all incomming events to be queued, and store the resulting connection
    m_connection(eventHub->subscribeToAll(
        boost::bind(&QueuedEventHubImp::queueEvent, m_imp.get(), _1))),
    m_waitUpdate(false)
{
    m_imp->setPublishFunction(boost::bind(&QueuedEventHub::_publish, this, _1));
}


QueuedEventHub::QueuedEventHub(ConfigNode config, SubsystemList deps) :
    EventHub(config["name"].asString()),
    m_hub(core::Subsystem::getSubsystemOfType<EventHub>(deps)),
    m_imp(new QueuedEventHubImp()),
    // Send all incomming events to be queued and store the resulting connection
    m_connection(m_hub->subscribeToAll(
        boost::bind(&QueuedEventHubImp::queueEvent, m_imp, _1))),
    m_waitUpdate(false)
{
    m_imp->setPublishFunction(boost::bind(&QueuedEventHub::_publish, this, _1));
}

QueuedEventHub::~QueuedEventHub()
{
    m_connection->disconnect();
}

void QueuedEventHub::setWaitUpdate(bool value)
{
    m_waitUpdate = value;
}

void QueuedEventHub::publish(EventPtr event)
{
    m_imp->queueEvent(event);
}

void QueuedEventHub::publish(Event::EventType etype, EventPtr event)
{
    event->type = etype;
    event->sender = this;
    publish(event);
}
    
int QueuedEventHub::publishEvents()
{
    return m_imp->publishEvents();
}
    
int QueuedEventHub::waitAndPublishEvents()
{
    return m_imp->waitAndPublishEvents();
}
    
void QueuedEventHub::update(double)
{
    if (m_waitUpdate)
        waitAndPublishEvents();
    else
        publishEvents();
}

void QueuedEventHub::_publish(EventPtr event)
{
    EventHub::publish(event);
}

} // namespace core
} // namespace ram

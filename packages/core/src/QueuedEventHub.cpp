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
    m_imp(new QueuedEventHubImp()),
    // Send all incomming events to be queued, and store the resulting connection
    m_connection(eventHub->subscribeToAll(
        boost::bind(&QueuedEventHubImp::queueEvent, m_imp.get(), _1)))
{
    m_imp->setPublishFunction(boost::bind(&QueuedEventHub::_publish, this, _1));
}


QueuedEventHub::QueuedEventHub(ConfigNode config, SubsystemList deps) :
    EventHub(config["name"].asString()),
    m_imp(new QueuedEventHubImp()),
    // Send all incomming events to be queued, and store the resulting connection
    m_connection(
        core::Subsystem::getSubsystemOfType<EventHub>(deps)->subscribeToAll(
            boost::bind(&QueuedEventHubImp::queueEvent, m_imp, _1)))
{
    m_imp->setPublishFunction(boost::bind(&QueuedEventHub::_publish, this, _1));
}

QueuedEventHub::~QueuedEventHub()
{
    m_connection->disconnect();
}

void QueuedEventHub::publish(EventPtr event)
{
    m_imp->queueEvent(event);
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
    m_imp->publishEvents();
}

void QueuedEventHub::background(int)
{
}

void QueuedEventHub::unbackground(bool)
{
}

bool QueuedEventHub::backgrounded()
{
    return false;
}

void QueuedEventHub::_publish(EventPtr event)
{
    EventHub::publish(event);
}

} // namespace core
} // namespace ram

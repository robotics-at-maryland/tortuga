/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/logging/src/EventLogger.cpp
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

// STD Includes
#include <iostream>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "logging/include/EventLogger.h"
#include "logging/include/Serialize.h"

#include "core/include/EventConnection.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/Logging.h"
#include "core/include/EventHub.h"
#include "core/include/Events.h"
// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::logging::EventLogger, EventLogger);

namespace ram {
namespace logging {

EventLogger::EventLogger(core::ConfigNode config) :
    Subsystem(config["name"].asString("EventLogger")),
    m_archive(0)//m_logFile)
{
    init(config, core::SubsystemList());
}
    
EventLogger::EventLogger(core::ConfigNode config, core::SubsystemList deps) :
    Subsystem(config["name"].asString("EventLogger"), deps),
    m_archive(0)//m_logFile)
{
    init(config, deps);
}

EventLogger::~EventLogger()
{
    // Disconnect the event connection
    m_connection->disconnect();

    // Flush the log to disk
    core::EventPtr event;
    while(m_eventQueue.popNoWait(event))
        writeEvent(event);

    // Close the log file
    m_logFile.close();
}

void EventLogger::update(double)
{
    // Read off events and write them to disk
    core::EventPtr event;
    
    while(m_eventQueue.popNoWait(event))
        writeEvent(event);
}

void EventLogger::setPriority(core::IUpdatable::Priority priority)
{
    Updatable::setPriority(priority);
}
    
core::IUpdatable::Priority EventLogger::getPriority()
{
    return Updatable::getPriority();
}

void EventLogger::setAffinity(size_t affinity)
{
    Updatable::setAffinity(affinity);
}
    
int EventLogger::getAffinity()
{
    return Updatable::getAffinity();
}
    
void EventLogger::background(int interval)
{
    Updatable::background(interval);
}
    
void EventLogger::unbackground(bool join)
{
    Updatable::unbackground(join);
}

bool EventLogger::backgrounded()
{
    return Updatable::backgrounded();
}

void EventLogger::init(core::ConfigNode config, core::SubsystemList deps)
{
    // Open our log file
    std::string fileName = config["fileName"].asString("event.log");
    std::string filePath = (core::Logging::getLogDir() / fileName).string();
    m_logFile.open(filePath.c_str(), 
  		   std::ios::out | std::ios::app | std::ios::binary);

    // Create our archive
    m_archive = new boost::archive::text_oarchive(m_logFile);

    // Register all types with the archive
    ram::logging::registerTypes(*m_archive);

    // Get our subsystem
    core::EventHubPtr eventHub =
         core::Subsystem::getSubsystemOfType<core::EventHub>(deps);

    // Subscribe to recieve all events
    m_connection = eventHub->subscribeToAll(
        boost::bind(&EventLogger::queueEvent, this, _1));
}

void EventLogger::queueEvent(core::EventPtr event)
{
    // Queue up the event so it will get logged to disk in the background
    m_eventQueue.push(event);
}

void EventLogger::writeEvent(core::EventPtr event)
{
    std::string typeName(typeid(*(event.get())).name());
    try
    {
        // Only attempt to convert events we now we can convert
        if (m_unconvertableTypes.end() == m_unconvertableTypes.find(typeName))
            (*m_archive) << event;
    }
    catch (boost::archive::archive_exception ex)
    {
        if (ex.code == boost::archive::archive_exception::unregistered_class)
        {
            std::cerr << "Could not convert: " << typeName << event->type
                      << std::endl;
            m_unconvertableTypes.insert(typeName);
        }
        else
        {
            throw ex;
        }
    }
}
    
} // namespace logging
} // namespace ram

/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/logging/src/EventPlayer.cpp
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

// STD Includes
#include <iostream>
#include <iomanip>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "logging/include/EventPlayer.h"
#include "logging/include/Serialize.h"

#include "core/include/EventConnection.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/Logging.h"
#include "core/include/EventHub.h"
#include "core/include/Events.h"
#include "core/include/TimeVal.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::logging::EventPlayer, EventPlayer);

RAM_CORE_EVENT_TYPE(ram::logging::EventPlayer, START);
RAM_CORE_EVENT_TYPE(ram::logging::EventPlayer, STOP);

namespace ram {
namespace logging {

EventPlayer::EventPlayer(core::ConfigNode config) :
    Subsystem(config["name"].asString("EventPlayer")),
    m_startTime(core::TimeVal::timeOfDay().get_double()),
    m_archive(0), //m_logFile)
    m_firstEventTime(-1),
    m_currentTime(-1),
    m_stoppedTime(-1),
    m_stopageTime(0),
    m_fileLength(-1)
{
    init(config, core::SubsystemList());
}
    
EventPlayer::EventPlayer(core::ConfigNode config, core::SubsystemList deps) :
    Subsystem(config["name"].asString("EventPlayer"), deps),
    m_startTime(core::TimeVal::timeOfDay().get_double()),
    m_archive(0), //m_logFile)
    m_firstEventTime(-1),
    m_currentTime(-1),
    m_stoppedTime(-1),
    m_stopageTime(0),
    m_fileLength(-1)
{
    init(config, deps);
}

EventPlayer::~EventPlayer()
{
    // Close the log file
    m_logFile.close();
}

double EventPlayer::duration()
{
    return 0;
}

void EventPlayer::seekToTime(double seconds)
{
    // TODO
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);

        // Round the down to the nearest second
        
        // Seek to that time by reading out lots of events

        // Move forward through file until we are at the proper time
    }
}

double EventPlayer::currentTime()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_currentTime;
}

void EventPlayer::start()
{
    {
        // Determine how much time we have been stopped
        core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
        double timeStopped = getTimeOfDay() - m_stoppedTime;

        // Add that to our stopage time so playback still works
        m_stopageTime += timeStopped;

        // Now start backup the background thread
        background(-1);
    }

    publish(START, core::EventPtr(new core::Event()));
}

void EventPlayer::stop()
{
    // Stop the background thread
    unbackground(true);;

    // Record when stopped playback
    {
        core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
        m_stoppedTime = getTimeOfDay();
    }

    publish(STOP, core::EventPtr(new core::Event()));
}

    
void EventPlayer::update(double)
{
    // While there are still events left in the log
    if (m_logFile.tellg() < m_fileLength)
    {
        ram::core::EventPtr event = ram::core::EventPtr();
        double sendTime = 0;
        {
            core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
            
            // Grab event from the log
            (*m_archive) >> event;

            // Grab our first event time if needed
            if (-1 == m_firstEventTime)
            {
                m_firstEventTime = event->timeStamp;
            }

            // Grab essentially the place we are in the log file
            double delta = event->timeStamp - m_firstEventTime;
            m_currentTime = delta;
            sendTime = m_startTime + delta + m_stopageTime;
        }

        // If in the "past" send the event, other wise sleep until it must
        // be sent out
        double now = getTimeOfDay();
        while (now < sendTime)
        {
            // Compute the time needed to sleep in seconds
            double sleepTime = sendTime - now;
            eventSleep(sleepTime);
            now = getTimeOfDay();
        }
        
        // Clone the event to send
        core::EventPtr eventToSend(event->clone());
        eventToSend->timeStamp = sendTime;
            
        if (eventToSend->sender)
        {
            // Republish the event with the events sender
            eventToSend->sender->publish(eventToSend->type, eventToSend);
        }
        else
        {
            // Republish just to the event hub
            m_eventHub->publish(eventToSend);
        }
    } // If we aren't at the end of the log file
}

void EventPlayer::setPriority(core::IUpdatable::Priority priority)
{
    Updatable::setPriority(priority);
}
    
core::IUpdatable::Priority EventPlayer::getPriority()
{
    return Updatable::getPriority();
}

void EventPlayer::setAffinity(size_t affinity)
{
    Updatable::setAffinity(affinity);
}
    
int EventPlayer::getAffinity()
{
    return Updatable::getAffinity();
}
    
void EventPlayer::background(int interval)
{
    Updatable::background(interval);
}
    
void EventPlayer::unbackground(bool join)
{
    Updatable::unbackground(join);
}

bool EventPlayer::backgrounded()
{
    return Updatable::backgrounded();
}

double EventPlayer::getTimeOfDay()
{
    return core::TimeVal::timeOfDay().get_double();
}
    
void EventPlayer::eventSleep(double seconds)
{
    // Sleep (note the time is converted to microseconds
    waitForUpdate((long)(seconds * 1000000));
}


void EventPlayer::init(core::ConfigNode config, core::SubsystemList deps)
{
    // Open our log file
    std::string fileName = config["fileName"].asString("event.log");
    m_logFile.open(fileName.c_str());

    assert(m_logFile.is_open() && "Could not open log file");
    
    // Get length of file:  
    m_logFile.seekg (0, std::ios::end);
    m_fileLength = m_logFile.tellg();
    m_logFile.seekg (0, std::ios::beg);

    // Create our archive
    m_archive = new boost::archive::text_iarchive(m_logFile);
    
    // Get our subsystem
    m_eventHub = core::Subsystem::getSubsystemOfType<core::EventHub>(deps);
}
    
} // namespace logging
} // namespace ram

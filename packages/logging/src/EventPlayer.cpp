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
#include <vector>

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
RAM_CORE_EVENT_TYPE(ram::logging::EventPlayer, PLAYER_UPDATE);
RAM_CORE_EVENT_TYPE(ram::logging::EventPlayer, PLAYER_SETUP);

namespace ram {
namespace logging {

EventPlayer::EventPlayer(core::ConfigNode config) :
    Subsystem(config["name"].asString("EventPlayer"))
{
    m_playerThread = new PlayerThread::PlayerThread(config, this);
}

EventPlayer::EventPlayer(core::ConfigNode config, core::SubsystemList deps) :
    Subsystem(config["name"].asString("EventPlayer"), deps)
{
    m_playerThread = new PlayerThread::PlayerThread(config, deps, this);
}

EventPlayer::~EventPlayer()
{

}

void EventPlayer::start()
{
    m_playerThread->start();
}

void EventPlayer::stop()
{
    m_playerThread->stop();
}

double EventPlayer::currentTime()
{
    return m_playerThread->currentTime();
}

double EventPlayer::duration()
{
    return m_playerThread->duration();
}

void EventPlayer::seekToTime(double seconds)
{
    m_playerThread->seekToTime(seconds);
}

void EventPlayer::background(int interval)
{

}

void EventPlayer::unbackground(bool join)
{
    
}

bool EventPlayer::backgrounded()
{
    return true;
}

void EventPlayer::setPriority(core::IUpdatable::Priority priority)
{
}
    
core::IUpdatable::Priority EventPlayer::getPriority()
{
    return IUpdatable::NORMAL_PRIORITY;
}

void EventPlayer::setAffinity(size_t)
{
}
    
int EventPlayer::getAffinity()
{
    return -1;
}

void EventPlayer::update(double)
{

}


void EventPlayer::publishStart()
{
    publish(START, core::EventPtr(new core::Event()));
}

void EventPlayer::publishStop()
{
    publish(STOP, core::EventPtr(new core::Event()));
}

void EventPlayer::publishSetup()
{
    publish(PLAYER_SETUP, core::EventPtr(new core::Event()));
}

void EventPlayer::publishUpdate()
{
    publish(PLAYER_UPDATE, core::EventPtr(new core::Event()));
}
    

PlayerThread::PlayerThread(core::ConfigNode config, EventPlayer *player) :
    m_startTime(core::TimeVal::timeOfDay().get_double()),
    m_archive(0), //m_logFile)
    m_firstEventTime(-1),
    m_currentTime(-1),
    m_stoppedTime(-1),
    m_stopageTime(0),
    m_fileLength(-1),
    m_presentEvent(0)
{
    m_player = player;
    init(config, core::SubsystemList());
}
    
PlayerThread::PlayerThread(core::ConfigNode config, core::SubsystemList deps, EventPlayer *player) :
    m_startTime(core::TimeVal::timeOfDay().get_double()),
    m_archive(0), //m_logFile)
    m_firstEventTime(-1),
    m_currentTime(-1),
    m_stoppedTime(-1),
    m_stopageTime(0),
    m_fileLength(-1),
    m_presentEvent(0)
{
    m_player = player;
    init(config, deps);
}

PlayerThread::~PlayerThread()
{
    // Close the log file
    m_logFile.close();
}

double PlayerThread::duration()
{
    return m_duration;
}

void PlayerThread::seekToTime(double seconds)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
        m_stopageTime += m_currentTime - seconds;
        m_currentTime = seconds;
        for(int i=0; i < (int) m_pastEvents.size(); i++){
            if((m_pastEvents.at(i)->timeStamp) >= seconds){
                m_presentEvent = i;
                return;
            }
        }
    }
}

double PlayerThread::currentTime()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_currentTime;
}

void PlayerThread::start()
{
    {
        // Determine how much time we have been stopped
        core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
        if(m_stoppedTime != -1) {
            double timeStopped = getTimeOfDay() - m_stoppedTime;
        
            // Add that to our stopage time so playback still works
            m_stopageTime += timeStopped;
        }

        // Now start backup the background thread
        background(-1);
    }

    m_player->publishStart();
}

void PlayerThread::stop()
{
    // Stop the background thread
    unbackground(true);;

    // Record when stopped playback
    {
        core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
        m_stoppedTime = getTimeOfDay();
    }

    m_player->publishStop();
}

    
void PlayerThread::update(double)
{
    // If the "current event" is in the pastEvents vector
    if(m_pastEvents.size() > m_presentEvent){

        ram::core::EventPtr event = m_pastEvents.at(m_presentEvent);

        // Grab essentially the place we are in the log file
        double delta = event->timeStamp;
        m_currentTime = delta;
        double sendTime = m_startTime + delta + m_stopageTime;

        // If in the "past" send the event, other wise sleep until it must
        // be sent out
        double now = getTimeOfDay();
        now -= m_stopageTime;
        while (now < (m_currentTime + m_startTime))
        {
            // Compute the time needed to sleep in seconds
            double sleepTime = m_currentTime + m_startTime - now;
            eventSleep(sleepTime);
            now = getTimeOfDay() - m_stopageTime;
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
        m_player->publishUpdate();
        m_presentEvent++;
    }
}

void PlayerThread::setPriority(core::IUpdatable::Priority priority)
{
    Updatable::setPriority(priority);
}
    
core::IUpdatable::Priority PlayerThread::getPriority()
{
    return Updatable::getPriority();
}

void PlayerThread::setAffinity(size_t affinity)
{
    Updatable::setAffinity(affinity);
}
    
int PlayerThread::getAffinity()
{
    return Updatable::getAffinity();
}
    
void PlayerThread::background(int interval)
{
    Updatable::background(interval);
}
    
void PlayerThread::unbackground(bool join)
{
    Updatable::unbackground(join);
}

bool PlayerThread::backgrounded()
{
    return Updatable::backgrounded();
}

double PlayerThread::getTimeOfDay()
{
    return core::TimeVal::timeOfDay().get_double();
}
    
void PlayerThread::eventSleep(double seconds)
{
    // Sleep (note the time is converted to microseconds
    waitForUpdate((long)(seconds * 1000000));
}


void PlayerThread::init(core::ConfigNode config, core::SubsystemList deps)
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

    // Add all of the events in the file to a Vector
    while(m_logFile.tellg() < m_fileLength) {
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
            //sendTime = m_startTime + delta;
            sendTime = delta;
        }
        core::EventPtr eventToAdd(event->clone());
        eventToAdd->timeStamp = sendTime;
        m_pastEvents.push_back(eventToAdd);
    }
    // If there are events in the file, duration is last timstamp
    m_duration = 0;
    if(m_pastEvents.size()){
        m_duration = m_pastEvents.at(m_pastEvents.size() - 1)->timeStamp;
    }
    m_player->publishSetup();
}
    
} // namespace logging
} // namespace ram

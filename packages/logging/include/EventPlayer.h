/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/logging/include/EventPlayer.h
 */

#ifndef RAM_LOGGING_EVENTPLAYER_07_06_2009
#define RAM_LOGGING_EVENTPLAYER_07_06_2009

// STD Includes
#include <fstream>
#include <set>
#include <vector>

// Library Includes
#include <boost/archive/text_iarchive.hpp>

// Project Includes
#include "core/include/Forward.h"
#include "core/include/Subsystem.h"
#include "core/include/Updatable.h"
#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"
#include "logging/include/Common.h"

namespace ram {
namespace logging {

class EventPlayer;

/** Plays back events from a log file at the rate the were really played */
class PlayerThread : public core::Updatable
{
public:
    
    PlayerThread(core::ConfigNode config, EventPlayer *player);
    PlayerThread(core::ConfigNode config, core::SubsystemList deps, EventPlayer *player);
    ~PlayerThread();

    /** Length of the log file */
    virtual double duration();

    /** Seek to a specific time in the log */
    virtual void seekToTime(double seconds);

    /** Get the current time in the log (in seconds since start) */
    virtual double currentTime();

    /** Stops the current playback */
    virtual void start();

    /** Starts the current event playback*/
    virtual void stop();
    
    // IUpdatable methods

    /** Sleeps until the next event from the file is ready to be broadcast */
    virtual void update(double);

    virtual void setPriority(core::IUpdatable::Priority priority);
    
    virtual core::IUpdatable::Priority getPriority();

    virtual void setAffinity(size_t affinity);
    
    virtual int getAffinity();
    
    virtual void background(int interval);
    
    virtual void unbackground(bool join = false);
    
    virtual bool backgrounded();

protected:
    virtual double getTimeOfDay();
    
    virtual void eventSleep(double seconds);

    /** The the subsystem started up */
    double m_startTime;
    
private:
    /** Creates all parts of the underlying logging system */
    void init(core::ConfigNode config, core::SubsystemList deps);

    /** The archive we reading from */
    boost::archive::text_iarchive* m_archive;

    /** The file we are writing the data to */
    std::ifstream m_logFile;

    /** Protects access to the file */
    core::ReadWriteMutex m_mutex;
    
    /** The time for the first event in the log file */
    double m_firstEventTime;
    
    /** The current time we are at*/
    double m_currentTime;

    /** The time at which the EventPlayer was stopped */
    double m_stoppedTime;

    /** How much time the player has been stopped (offsets the start time) */
    double m_stopageTime;

    /** The duration of a set of events. (The timestamp of the last event) */
    double m_duration;
    
    /** The length of the file we are reading */
    int m_fileLength;

    /** Our event hub that is usable */
    core::EventHubPtr m_eventHub;

    /** Events that we have already read from the logfile */
    std::vector<core::EventPtr> m_pastEvents;

    /** Location of the "present event" in the vector/logfile. Starting at the 
        vector*/
    unsigned int m_presentEvent;

    /** EventPlayer subsystem */
    EventPlayer *m_player;
};

class EventPlayer : public core::Subsystem
{
public:
    /** Sent when ever the player starts replaying events */
    static const core::Event::EventType START;
    
    /** Sent when ever the player stops replaying events */
    static const core::Event::EventType STOP;

    /** Sent when ever the player updates */
    static const core::Event::EventType PLAYER_UPDATE;

    /** Sent when the player has completed reading in the events */
    static const core::Event::EventType PLAYER_SETUP;


    EventPlayer(core::ConfigNode config);
    EventPlayer(core::ConfigNode config, core::SubsystemList deps);
    ~EventPlayer();

    /** Stops the current playback */
    virtual void start();

    /** Starts the current event playback*/
    virtual void stop();

    /** Gets the current Time from the player thread */
    virtual double currentTime();
    
    /** Length of the log file */
    virtual double duration();

    /** Seek to a specific time in the log */
    virtual void seekToTime(double seconds);

    virtual void background(int interval);
    
    virtual void unbackground(bool join = false);
    
    virtual bool backgrounded();

    virtual void update(double);

    virtual void setPriority(core::IUpdatable::Priority priority);
    
    virtual core::IUpdatable::Priority getPriority();

    virtual void setAffinity(size_t affinity);
    
    virtual int getAffinity();
    
    void publishStart();
    
    void publishStop();
    
    void publishSetup();
    
    void publishUpdate();

private:
    PlayerThread *m_playerThread;
};


} // namespace logging
} // namespace ram
    
#endif // RAM_LOGGING_EVENTPLAYER_07_06_2009

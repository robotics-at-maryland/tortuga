/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/logging/include/EventLogger.h
 */

#ifndef RAM_LOGGING_EVENTLOGGER_06_06_2009
#define RAM_LOGGING_EVENTLOGGER_06_06_2009

// STD Includes
#include <fstream>
#include <set>

// Library Includes
#include <boost/archive/text_oarchive.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/Updatable.h"
#include "core/include/ConfigNode.h"
#include "core/include/ThreadedQueue.h"

namespace ram {
namespace logging {

class EventLogger : public core::Subsystem, public core::Updatable
{
public:
    EventLogger(core::ConfigNode config);
    EventLogger(core::ConfigNode config, core::SubsystemList deps);
    ~EventLogger();

    // IUpdatable methods
    virtual void update(double);

    virtual void setPriority(core::IUpdatable::Priority priority);
    
    virtual core::IUpdatable::Priority getPriority();

    virtual void setAffinity(size_t affinity);
    
    virtual int getAffinity();
    
    virtual void background(int interval);
    
    virtual void unbackground(bool join = false);
    
    virtual bool backgrounded();
    
private:
    /** Creates all parts of the underlying logging system */
    void init(core::ConfigNode config, core::SubsystemList deps);

    /** Store event on the internal queue so it can be written to disk */
    void queueEvent(core::EventPtr event);

    /** Writes the event to disk through the archive */
    void writeEvent(core::EventPtr event);

    /** Connection for the recieved events */
    core::EventConnectionPtr m_connection;

    /** Holds queued events we are goign to log to the file */
    core::ThreadedQueue<core::EventPtr> m_eventQueue;

    /** The file we are writing the data to */
    std::ofstream m_logFile;
    
    /** The archive we are writing to */
    boost::archive::text_oarchive* m_archive;

    /** List of all type we cannot convert for some reason */
    std::set<std::string> m_unconvertableTypes;
};

} // namespace logging
} // namespace ram
    
#endif // RAM_LOGGING_EVENTLOGGER_06_06_2009

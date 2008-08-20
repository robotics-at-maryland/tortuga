/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/ThreadedAppender.h
 */

#ifndef RAM_CORE_THREADEDFILEAPPENDER_07_06_2008
#define RAM_CORE_THREADEDFILEAPPENDER_07_06_2008

// STD Includes
#include <string>

// Library Includes
#include <log4cpp/Appender.hh>
#include <log4cpp/LoggingEvent.hh>

// Project Includes
#include "core/include/Updatable.h"
#include "core/include/ThreadedQueue.h"

namespace ram {
namespace core {

/** A logger which logs all data in a background thread
 *
 *  Use of this Appender allows logging without stalling the thread doing the
 *  logging.  It uses the decorator pattern.
 */
class ThreadedAppender : public log4cpp::Appender, public Updatable
{
public:
    ThreadedAppender(log4cpp::Appender* appender);
    virtual ~ThreadedAppender();

    /** Waits for logging events and writes to files as possible*/        
    virtual void update(double timestep);

    /** Gets the appender we are wraping */
    log4cpp::Appender* wrappedAppender();
    
    /** Queues up logging event */
    virtual void doAppend(const log4cpp::LoggingEvent &event);

    // Normal methods just passed through to lower appender
    virtual bool reopen();
    virtual void close();
    virtual bool requiresLayout();
    virtual bool requiresLayout() const;
    virtual void setLayout(log4cpp::Layout *layout);
    const std::string & getName () const;
    virtual void setThreshold(log4cpp::Priority::Value priority);
    virtual log4cpp::Priority::Value getThreshold();
    virtual void setFilter(log4cpp::Filter *filter);
    virtual log4cpp::Filter* getFilter();
    
private:
    /** Queues up log events to be written to the file */
    ThreadedQueue<log4cpp::LoggingEvent> m_logEvents;

    /** The appender we are wrapper*/
    log4cpp::Appender* m_appender;
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_THREADEDFILEAPPENDER_07_06_2008

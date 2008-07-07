/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/ThreadedAppender.cpp
 */

// Project Includes
#include "core/include/ThreadedAppender.h"

namespace ram {
namespace core {

ThreadedAppender::ThreadedAppender(log4cpp::Appender* appender) :
    Appender(appender->getName()),
    m_appender(appender)
{
    // Start running full out
    background(-1);
}
    
ThreadedAppender::~ThreadedAppender()
{
    // Stop background thread
    unbackground(true);
    delete m_appender;
}

void ThreadedAppender::update(double timestep)
{
    log4cpp::LoggingEvent event("", "", "", 1);

    // Clear all current events
    while(m_logEvents.popNoWait(event))
        m_appender->doAppend(event);

    // Wait for half a second, log event if needed, then reloop
    boost::xtime wait ={0, 500000000}; // 500 milliseconds
    if(m_logEvents.popTimedWait(wait, event))
        m_appender->doAppend(event);
}

log4cpp::Appender* ThreadedAppender::wrappedAppender()
{
    return m_appender;
}
    
void ThreadedAppender::doAppend(const log4cpp::LoggingEvent &event)
{
    // Queue up event
    m_logEvents.push(event);
}

bool ThreadedAppender::reopen()
{
    return m_appender->reopen();
}
    
void ThreadedAppender::close()
{
    m_appender->close();
}
    
bool ThreadedAppender::requiresLayout()
{
    return m_appender->requiresLayout();
}

bool ThreadedAppender::requiresLayout() const
{
    return m_appender->requiresLayout();
}
    
void ThreadedAppender::setLayout(log4cpp::Layout *layout)
{
    m_appender->setLayout(layout);
}
    
const std::string& ThreadedAppender::getName() const
{
    return m_appender->getName();
}
    
void ThreadedAppender::setThreshold(log4cpp::Priority::Value priority)
{
    m_appender->setThreshold(priority);
}
    
log4cpp::Priority::Value ThreadedAppender::getThreshold()
{
    return m_appender->getThreshold();
}
    
void ThreadedAppender::setFilter(log4cpp::Filter *filter)
{
    m_appender->setFilter(filter);
}

log4cpp::Filter* ThreadedAppender::getFilter()
{
    return m_appender->getFilter();
}
    
} // namespace core
} // namespace ram

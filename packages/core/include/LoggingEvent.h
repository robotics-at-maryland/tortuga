/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Leo Singer <aronnax@umd.edu>
 * File:  packages/core/include/LoggingEvent.h
 */

#ifndef RAM_CORE_LOGGINGEVENT_H_09_01_2008
#define RAM_CORE_LOGGINGEVENT_H_09_01_2008

// Library Includes
#include <log4cpp/LoggingEvent.hh>

namespace ram {
namespace core {

/** Subclass of standard Log4Cpp event to allow copying */
struct LoggingEvent : log4cpp::LoggingEvent
{
    /** Standard Log4cpp constructor see there docs */
    inline LoggingEvent(const std::string& category,
                        const std::string& message, 
                        const std::string& ndc,
                        log4cpp::Priority::Value priority) :
    log4cpp::LoggingEvent(category, message, ndc, priority) {}

    /** Copy constructor */
    inline LoggingEvent(const LoggingEvent& other) :
        log4cpp::LoggingEvent(other.categoryName, other.message, other.ndc,
                              other.priority)
    {
        *(const_cast<std::string*>(&threadName)) = *(&(other.threadName));
        priority = other.priority;
        timeStamp = other.timeStamp;
    }

    /** Allows copy constructing from a log4cpp::LoggingEvent */
    inline LoggingEvent(const log4cpp::LoggingEvent& other) :
        log4cpp::LoggingEvent(other.categoryName, other.message, other.ndc,
                              other.priority)
    {
        *(const_cast<std::string*>(&threadName)) = *(&(other.threadName));
        priority = other.priority;
        timeStamp = other.timeStamp;
    }

    /** Standard copy assignment operator */
    LoggingEvent& operator=(const LoggingEvent& other)
    {
        *(const_cast<std::string*>(&categoryName)) = *(&(other.categoryName));
        *(const_cast<std::string*>(&message)) = *(&(other.message));
        *(const_cast<std::string*>(&ndc)) = *(&(other.ndc));
        *(const_cast<std::string*>(&threadName)) = *(&(other.threadName));
        priority = other.priority;
        timeStamp = other.timeStamp;
        return *this;
    }

    /** Allows conversion to log4cpp::LoggingEvent for better integration */
    operator log4cpp::LoggingEvent()
    {
        log4cpp::LoggingEvent lge(categoryName, message, ndc, priority);
        *(const_cast<std::string*>(&(lge.threadName))) = *(&(threadName));
        lge.timeStamp = timeStamp;
        return lge;
    }
    
};
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_LOGGINGEVENT_H_09_01_2008


/*
 * LoggingEvent.hh
 * with copy constructors added by Leo Singer <aronnax@umd.edu>
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _RAM_CORE_LOGGINGEVENT_H
#define _RAM_CORE_LOGGINGEVENT_H

#include <log4cpp/LoggingEvent.hh>

/**
 * The top level namespace for all 'Log for C++' types and classes.
 **/
namespace ram { namespace core {

    /**
     * The internal representation of logging events. When a affirmative
     * logging decision is made a <code>LoggingEvent</code> instance is
     * created. This instance is passed around the different log4cpp
     * components.
     *
     * <p>This class is of concern to those wishing to extend log4cpp. 
     **/
    struct LoggingEvent : log4cpp::LoggingEvent {
    public:
        /**
         * Instantiate a LoggingEvent from the supplied parameters.
         *
         * <p>Except <code>timeStamp</code> all the other fields of
         * <code>LoggingEvent</code> are filled when actually needed.
         * <p>
         * @param category The category of this event.
         * @param message  The message of this event.
         * @param ndc The nested diagnostic context of this event. 
         * @param priority The priority of this event.
         **/
        inline LoggingEvent(const std::string& category, const std::string& message, 
                            const std::string& ndc, log4cpp::Priority::Value priority) :
        log4cpp::LoggingEvent(category, message, ndc, priority) {}
        
        inline LoggingEvent(const LoggingEvent& other) :
        log4cpp::LoggingEvent(other.categoryName, other.message, other.ndc, other.priority)
        {
            *(const_cast<std::string*>(&threadName)) = *(&(other.threadName));
            priority = other.priority;
            timeStamp = other.timeStamp;
        }
        
        inline LoggingEvent(const log4cpp::LoggingEvent& other) :
        log4cpp::LoggingEvent(other.categoryName, other.message, other.ndc, other.priority)
        {
            *(const_cast<std::string*>(&threadName)) = *(&(other.threadName));
            priority = other.priority;
            timeStamp = other.timeStamp;
        }
        
        LoggingEvent& LoggingEvent::operator=(const LoggingEvent& other)
        {
            *(const_cast<std::string*>(&categoryName)) = *(&(other.categoryName));
            *(const_cast<std::string*>(&message)) = *(&(other.message));
            *(const_cast<std::string*>(&ndc)) = *(&(other.ndc));
            *(const_cast<std::string*>(&threadName)) = *(&(other.threadName));
            priority = other.priority;
            timeStamp = other.timeStamp;
            return *this;
        }
        
        operator log4cpp::LoggingEvent()
        {
            log4cpp::LoggingEvent lge(categoryName, message, ndc, priority);
            *(const_cast<std::string*>(&(lge.threadName))) = *(&(threadName));
            lge.timeStamp = timeStamp;
            return lge;
        }
        
    };
}}

#endif // _RAM_CORE_LOGGINGEVENT_HH


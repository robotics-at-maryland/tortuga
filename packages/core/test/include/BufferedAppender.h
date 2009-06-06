/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/include/BufferedAppender.h
 */

// STD Includes
#include <vector>

// Library Includes
#include <log4cpp/LayoutAppender.hh>

/** Buffers all recieved log events, used for testing below */
class BufferedAppender : public log4cpp::LayoutAppender
{
public:
    BufferedAppender(std::string name) : LayoutAppender(name) {}

    virtual void close() { logEvents.clear(); }
          
    std::vector<log4cpp::LoggingEvent> logEvents;
    
protected:
    virtual void _append(const log4cpp::LoggingEvent& event)
    {
        logEvents.push_back(event);
    }
};

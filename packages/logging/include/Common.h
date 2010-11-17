/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/logging/include/Common.h
 */

#ifndef RAM_LOGGING_COMMON_10_16_2010
#define RAM_LOGGING_COMMON_10_16_2010

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace logging {

class EventLogger;
typedef boost::shared_ptr<EventLogger> EventLoggerPtr;

class EventPlayer;
typedef boost::shared_ptr<EventPlayer> EventPlayerPtr; 

} // namespace logging
} // namespace ram

#endif // RAM_LOGGING_COMMON_10_16_2010

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Logging.h
 */

#ifndef RAM_CORE_LOGGING_07_04_2008
#define RAM_CORE_LOGGING_07_04_2008

// Library Includes
#include <log4cpp/Category.hh>

#define RAM_CORE_LOGGER_STRINGIFY(S) #S
#define RAM_CORE_LOGGER_STR(S) RAM_CORE_LOGGER_STRINGIFY(S)

#define RAM_CORE_MAKE_LOGGER(loggerName, catName) \
static log4cpp::Category* loggerName = \
    &(log4cpp::Category::getInstance(catName));

    
#endif // RAM_CORE_LOGGING_07_04_2008

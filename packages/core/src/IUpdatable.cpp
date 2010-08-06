/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/IUpdatable.cpp
 */

// STD Incldues
#include <map>

// Library Includes
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>

// Project Includes
#include "core/include/IUpdatable.h"

namespace ram {
namespace core {

typedef std::map<std::string, IUpdatable::Priority> StringPriorityMap;
IUpdatable::Priority IUpdatable::stringToPriority(std::string str)
{
    // Maps string to priority
    static StringPriorityMap str2Priority = boost::assign::map_list_of
        ("rt_high", IUpdatable::RT_HIGH_PRIORITY)
        ("rt_normal", IUpdatable::RT_NORMAL_PRIORITY)
        ("rt_low", IUpdatable::RT_LOW_PRIORITY)
        ("high", IUpdatable::HIGH_PRIORITY)
        ("normal", IUpdatable::NORMAL_PRIORITY)
        ("low", IUpdatable::LOW_PRIORITY);
    
    // Make it lower case
    boost::algorithm::to_lower(str);

    StringPriorityMap::iterator iter = str2Priority.find(str);
    assert(iter != str2Priority.end());

    return iter->second;
}

} // namepsace core
} // namespace ram

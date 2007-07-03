/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/ConfigNodeImp.h
 */

#ifndef RAM_CORE_CONFIGNODEIMP_06_30_2006
#define RAM_CORE_CONFIGNODEIMP_06_30_2006

// STD Includes
#include <string>

// Project Includes
#include "core/include/Common.h"

namespace ram {
namespace core {

// STD Includes
#include <string>

class ConfigNodeImp
{
public:
    virtual ~ConfigNodeImp() {};
    
    /** Grab a section of the config like an array */
    virtual ConfigNodeImpPtr idx(int index) = 0;

    /** Grab a sub node with the same name */
    virtual ConfigNodeImpPtr map(std::string key) = 0;

    /** Convert the node to a string value */
    virtual std::string asString() = 0;

    /** Convert the node to a double */
    virtual double asDouble() = 0;

    /** Convert the node to an int */
    virtual int asInt() = 0;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_CONFIGNODEIMP_06_30_2006




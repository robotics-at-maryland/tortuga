/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/ConfigNode.h
 */

#ifndef RAM_CORE_CONFIGNODE_06_30_2007
#define RAM_CORE_CONFIGNODE_06_30_2007

// STD Includes
#include <string>

// Project Includes
#include "core/include/Common.h"

namespace ram {
namespace core {

class ConfigNode
{
public:
    /** Use the given implementation */
    ConfigNode(ConfigNodeImpPtr impl);

    /** Copy constructor */
    ConfigNode(const ConfigNode& configNode);
    
    /** Grab a section of the config like an array */
    ConfigNode operator[](int index);

    /** Grab a sub node with the same name */
    ConfigNode operator[](std::string map);

    /** Convert the node to a string value */
    std::string asString();

    /** Attempts conversion to string, if it fails return def */
    std::string asString(const std::string& def);
    
    /** Convert the node to a double */
    double asDouble();

    /** Attempts conversion to string, if it fails return def */
    double asDouble(const double def);

    /** Convert the node to an int */
    int asInt();

    /** Attempts conversion to int, if it fails return def */
    int asInt(const int def);
    
    /** Builds a config node from the given string, this uses the python ver. */
    static ConfigNode fromString(std::string data);
    
private:
    ConfigNode();
    ConfigNode& operator=(const ConfigNode& that);
    
    ConfigNodeImpPtr m_impl;
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_CONFIG_06_30_2007

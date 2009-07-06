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
#include <set>

// Project Includes
#include "core/include/Common.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {
    
typedef std::set<std::string> NodeNameList ;
typedef NodeNameList::iterator NodeNameListIter;

class RAM_EXPORT ConfigNode
{
public:
    /** <b>DO NOT USE THIS</b> */
    ConfigNode();
    
    ConfigNode& operator=(const ConfigNode& that);
    
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

    /** Returns the list of sub nodes of the current config node (must be map)*/
    NodeNameList subNodes();

    /** If an array, returns the number of sub elements */
    size_t size();
    
    /** Whether or not a subnode actually exists */
    bool exists(std::string name);
    
    /** Map a value to the the given string inside a config node */
    void set(std::string key, std::string str);

    /** Map a value to the the given int inside a config node */
    void set(std::string key, int value);
    
    /** Builds a config node from the given string, this uses the python ver. */
    static ConfigNode fromString(std::string data);

    /** Returns the config file in a python evalable format */
    std::string toString();

    /** Attempts to load the config from file, the extension determines the
     backend used (NOT YET!!! - see below)

     @warning: This currently assume yml files and python yaml library 
    */
    static ConfigNode fromFile(std::string fileName);

private:    
    ConfigNodeImpPtr m_impl;
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_CONFIG_06_30_2007

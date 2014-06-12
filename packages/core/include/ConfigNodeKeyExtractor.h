/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/ConfigNodeKeyExtractor.h
 */

#ifndef RAM_CORE_CONFIGNODEKEYEXTRACTOR_09_29_2007
#define RAM_CORE_CONFIGNODEKEYEXTRACTOR_09_29_2007

// STD Includes
#include <string>

// Project Includes
#include "core/include/ConfigNode.h"

namespace ram {
namespace core {
    
/** Extracts a string Key From the Given Config Node */
struct ConfigNodeKeyExtractor
{
    static std::string extractKey(ConfigNode& node) {
        return node["type"].asString();
    }
};
    
} // namespace ram
} // namespace core

#endif // CONFIGNODEKEYEXTRACTOR_09_29_2007

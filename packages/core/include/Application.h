/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Application.h
 */

#ifndef RAM_CORE_APPLICATION_H_11_24_2007
#define RAM_CORE_APPLICATION_H_11_24_2007

// STD Includes
#include <string>
#include <vector>
#include <map>

// Project Includes
#include "core/include/Subsystem.h"

namespace ram {
namespace core {

typedef std::map<std::string, SubsystemPtr> NameSubsystemMap;
typedef NameSubsystemMap::iterator NameSubsystemMapIter;
    
/** Handles orderly subsystem creation, and destruction
 */
class Application
{
public:
    /** Starts up the application with the Subsystem definied in the given
        config file
    */
    Application(std::string configPath = "");

    /** Destroys all subsystems in the opposite order of there construction */
    ~Application();
    
    /** Returns the subsystem with the given name */
    SubsystemPtr getSubsystem(std::string name);

    /** Get the name of all subsystems */
    std::vector<std::string> getSubsystemName();
    
private:
    NameSubsystemMap m_subsystems;
    
    /// The order the systems were created in
    std::vector<std::string> m_order;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_APPLICATION_H_11_24_2007

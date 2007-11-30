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
#include <ostream>
#include <string>
#include <list>
#include <vector>
#include <map>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/ConfigNode.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

typedef std::map<std::string, SubsystemPtr> NameSubsystemMap;
typedef NameSubsystemMap::iterator NameSubsystemMapIter;
    
/** Handles orderly subsystem creation, and destruction
 */
class RAM_EXPORT Application
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
    std::vector<std::string> getSubsystemNames();

    /** Writes the dependency graph in Graphviz dot format */
    void writeDependencyGraph(std::ostream& file);
    
private:
    typedef std::vector<std::string> NameList;
    
    /** Does all the work to determine in what order we should start up
     subsystems using a topological sort and BGL */
    void determineStartupOrder(NodeNameList& subnodes, ConfigNode sysConfig);
    
    NameSubsystemMap m_subsystems;
    
    /** The order the systems are created in */
    NameList m_order;

    /** The names of which subsystem by each subsystem  */
    std::map<std::string, NameList> m_subsystemDeps;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_APPLICATION_H_11_24_2007

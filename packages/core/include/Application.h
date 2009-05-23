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
#include "core/include/ConfigNode.h"
#include "core/include/TimeVal.h"

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

    /** Starts updating non-backgrounded Subsystems in a blocking fashion
     *
     *  Every Subsystem which returns false from backgrounded(), will be
     *  updated in a continous loop, as fast as possible.  It will stop when
     *  stopMainLoop() is called.
     *
     *  @param singleSubsystem
     *      If true, the system will only run if a single subsystem is
     *      backgrounded at a time, this helps catch bugs related to process
     *      running in the main loop when they shouldn't.
     *
     */
    void mainLoop(bool singleSubsystem = false);

    /** Stops the loop started by the mainLoop() */
    void stopMainLoop();
    
private:
    typedef std::vector<std::string> NameList;
    
    /** Does all the work to determine in what order we should start up
     subsystems using a topological sort and BGL */
    void determineStartupOrder(NodeNameList& subnodes, ConfigNode sysConfig);

    /** Whether or not the main loop is running*/
    bool m_running;
    
    /** Stores subsystems, referened by name */
    NameSubsystemMap m_subsystems;
    
    /** The order the systems are created in */
    NameList m_order;

    /** Records when the the subsystem was last updated */
    std::map<std::string, TimeVal> m_lastUpdate;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_APPLICATION_H_11_24_2007

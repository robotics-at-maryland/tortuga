/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Application.cpp
 */
 
// STD Includes
#include <cassert>
#include <utility>
#include <set>
#include <map>
#include <sstream>
#include <exception>

// Library Includes
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// Project Includes
#include "core/include/Application.h"
#include "core/include/ConfigNode.h"
#include "core/include/Exception.h"
#include "core/include/Logging.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/DependencyGraph.h"
#include "core/include/Feature.h"

#ifdef RAM_WITH_WRAPPERS
#include <iostream>
#include <boost/python.hpp>

#define PYTHON_ERROR_TRY try
#define PYTHON_ERROR_CATCH(message)                     \
    catch(boost::python::error_already_set err) {       \
        std::cerr << "ERROR: " << message << std::endl; \
        PyErr_Print();                                  \
        throw err;                                      \
    }

#else
#define PYTHON_ERROR_TRY
#define PYTHON_ERROR_CATCH(message)
#endif

namespace ram {
namespace core {
    
Application::Application(std::string configPath) :
    m_running(false)
{
    boost::filesystem::path path(configPath);
    ConfigNode rootCfg = core::ConfigNode::fromFile(path.string());
    
    // Set creation mode
    std::string mode = rootCfg["SubsystemCreationMode"].asString("warning");
    if (mode != "error" && mode != "warning") {
        std::cout << "Invalid mode. Default is 'warning'" << std::endl;
        mode = "warning";
    }

    if (rootCfg.exists("Subsystems"))
    {
        ConfigNode sysConfig(rootCfg["Subsystems"]);

        // Properly fills m_order, and m_subsystemDeps
        DependencyGraph depGraph(sysConfig);
        m_order = depGraph.getOrder();

        std::vector<std::string> badSubsystemNames;
        std::set<std::string> invalidSystems;
        
        // Create all the subsystems
        BOOST_FOREACH(std::string subsystemName, m_order)
        {
            // Skip "creationMode"
            if (subsystemName == "creationMode") {
                continue;
            }

            // If the subsystem has no configuration section, ignore it
            if (!sysConfig.exists(subsystemName)) {
                badSubsystemNames.push_back(subsystemName);
                continue;
            }

            // Set 'name' properly in the config
            ConfigNode config(sysConfig[subsystemName]);
            config.set("name", subsystemName);
        
            // Build list of dependencies
            SubsystemList deps;
            NameList depNames = depGraph.getDependencies(subsystemName);
            bool abort = false;
            BOOST_FOREACH(std::string depName, depNames)
            {
                if (!hasSubsystem(depName) ||
                    invalidSystems.count(depName) == 1) {
                    // The dependencies have not been satisfied
                    abort = true;
                    break;
                }
                deps.push_back(getSubsystem(depName));
            }

            if (abort) {
                // The dependencies were not satisfied
                // do not make this subsystem
                // Remove from the order
                badSubsystemNames.push_back(subsystemName);
                continue;
            }

            // Create out new subsystem and store it
            PYTHON_ERROR_TRY {
                try {
                    SubsystemPtr subsystem(SubsystemMaker::newObject(
                                               std::make_pair(config, deps) ));
                    m_subsystems[subsystemName] = subsystem;
                } catch (core::MakerNotFoundException& ex) {
                    std::cout << ex.what() << " - "
                              << subsystemName << std::endl;
                    invalidSystems.insert(subsystemName);
                }
            } PYTHON_ERROR_CATCH("Subsystem construction");
        }
        
        // Add invalid systems to the bad subsystems
        BOOST_FOREACH(std::string name, invalidSystems)
        {
            badSubsystemNames.push_back(name);
        }
        
        // Remove bad subsystems from the order
        bool earlyTermination = false;
        BOOST_FOREACH(std::string name, badSubsystemNames)
        {
            // Three modes types
            if (mode == "error") {
                // End the program with an error state after
                // listing bad subsystems
                std::cout << "ERROR: Missing dependency " << name
                          << std::endl;
                earlyTermination = true;
            } else if (mode == "warning") {
                std::cout << "WARNING: Missing dependency "
                          << name << std::endl;
            }
            remove_from_order(name);
        }
        assert(!earlyTermination && "Dependencies are missing");

        // Not sure if this is the right place for this or not
        // maybe another function, maybe a scheduler?
        BOOST_FOREACH(std::string name, m_order)
        {
            PYTHON_ERROR_TRY {
                ConfigNode cfg(sysConfig[name]);
                if (cfg.exists("update_interval"))
                {
                    int updateInterval = cfg["update_interval"].asInt();
                    m_subsystems[name]->background(updateInterval);
                }
                
                if (cfg.exists("priority"))
                {
                    std::string priority(cfg["priority"].asString());
                    m_subsystems[name]->setPriority(
                        IUpdatable::stringToPriority(priority));
                }
                
                if (cfg.exists("affinity"))
                {
                    m_subsystems[name]->setAffinity(cfg["affinity"].asInt());
                }
            } PYTHON_ERROR_CATCH("Subsystem setup");
        } // foreach name in order
    } // if subsystem section of config exists
    
    // Write out the yaml config file in its current state
    rootCfg.writeToFile((Logging::getLogDir() /
                         "config.yml").native_file_string());
}

Application::~Application()
{
    PYTHON_ERROR_TRY {
        // Go through subsystems in the reverse order of construction and
        // shut them down
        for (int i = (((int)m_order.size()) - 1); i >= 0; --i)
        {
            std::string name(m_order[i]);
            SubsystemPtr subsystem = m_subsystems[name];
            subsystem->unbackground(true);
            
            m_subsystems.erase(name);
        }
    } PYTHON_ERROR_CATCH("Subsystem cleanup");
}

void Application::remove_from_order(std::string name)
{
    std::vector<std::string>::iterator iter =
        m_order.begin();
    for ( ; iter != m_order.end(); iter++) {
        if ((*iter) == name) {
            // Remove this subsystem
            m_order.erase(iter);
            // Don't need to continue to look
            break;
        }
    }
}

bool Application::hasSubsystem(std::string name)
{
    NameSubsystemMapIter iter = m_subsystems.find(name);
    return iter != m_subsystems.end();
}

SubsystemPtr Application::getSubsystem(std::string name)
{
    NameSubsystemMapIter iter = m_subsystems.find(name);
    assert(iter != m_subsystems.end() && "Error subsystem not found");
    return (*iter).second;
}

std::vector<std::string> Application::getSubsystemNames()
{
    return m_order;
}

void Application::mainLoop(bool singleSubsystem)
{
    m_running = true;
    
    typedef std::pair<std::string, SubsystemPtr> Pair;
    TimeVal now;

    // Run until stopMainLoop is called
    while (m_running)
    {
        int updated = 0;
        // Update each subsystem which isn't backgrounded
        BOOST_FOREACH(Pair item, m_subsystems)
        {
            SubsystemPtr subsystem = item.second;

            PYTHON_ERROR_TRY {
                if (!subsystem->backgrounded())
                {
                    updated++;
                    
                    now.now();
                    TimeVal timeSinceLastUpdate(now - m_lastUpdate[item.first]);
                    subsystem->update(timeSinceLastUpdate.get_double());
                    m_lastUpdate[item.first] = now;
                }
            } PYTHON_ERROR_CATCH("Subsystem loop");
        }

        assert(((updated == 1) || (!singleSubsystem)) &&
               "Single subsystem is updating multiple subsystems");
    }
}

void Application::stopMainLoop()
{
    m_running = false;
}
    
} // namespace core
} // namespace ram

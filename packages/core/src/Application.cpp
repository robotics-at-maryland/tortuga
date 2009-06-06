/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Application.cpp
 */

#ifdef RAM_WINDOWS
#pragma warning( disable : 4510 ) // Not default constuctor generated (BGL)
#pragma warning( disable : 4610 ) // Another caused by BGL
#endif
 
// STD Includes
#include <utility>
#include <map>

// Library Includes
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// Project Includes
#include "core/include/Application.h"
#include "core/include/ConfigNode.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/DependencyGraph.h"

namespace ram {
namespace core {
    
Application::Application(std::string configPath) :
    m_running(false)
{
    boost::filesystem::path path(configPath);
    ConfigNode rootCfg = core::ConfigNode::fromFile(path.string());

    
    if (rootCfg.exists("Subsystems"))
    {
        ConfigNode sysConfig(rootCfg["Subsystems"]);

        // Properly fills m_order, and m_subsystemDeps
        DependencyGraph depGraph(sysConfig);
        m_order = depGraph.getOrder();
        
        // Create all the subsystems
        BOOST_FOREACH(std::string subsystemName, m_order)
        {
            // Set 'name' properly in the config
            ConfigNode config(sysConfig[subsystemName]);
            config.set("name", subsystemName);

            // Build list of dependencies
            SubsystemList deps;
            NameList depNames = depGraph.getDependencies(subsystemName);
            BOOST_FOREACH(std::string depName, depNames)
                deps.push_back(getSubsystem(depName));

            // Create out new subsystem and store it
            SubsystemPtr subsystem(SubsystemMaker::newObject(
                std::make_pair(config, deps) ));
            
            m_subsystems[subsystemName] = subsystem;            
        }


        // Not sure if this is the right place for this or not
        // maybe another function, maybe a scheduler?
        BOOST_FOREACH(std::string name, m_order)
        {
            ConfigNode cfg(sysConfig[name]);
            if (cfg.exists("update_interval"))
                m_subsystems[name]->background(cfg["update_interval"].asInt());


            if (cfg.exists("priority"))
            {
                m_subsystems[name]->setPriority(
                    IUpdatable::stringToPriority(cfg["priority"].asString()));
            }

            if (cfg.exists("affinity"))
            {
                m_subsystems[name]->setAffinity(cfg["affinity"].asInt());
            }
        }
    }
}

Application::~Application()
{
    // Go through subsystems in the reverse order of contrustuction and shut
    // them down
    for (int i = (((int)m_order.size()) - 1); i >= 0; --i)
    {
        std::string name(m_order[i]);
        SubsystemPtr subsystem = m_subsystems[name];
        subsystem->unbackground(true);

        m_subsystems.erase(name);
    }
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

            if (!subsystem->backgrounded())
            {
                updated++;
                
                now.now();
                TimeVal timeSinceLastUpdate(now - m_lastUpdate[item.first]);
                subsystem->update(timeSinceLastUpdate.get_double());
                m_lastUpdate[item.first] = now;
            }
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

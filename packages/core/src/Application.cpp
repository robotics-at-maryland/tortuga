/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Application.cpp
 */

// STD Includes
#include <utility>

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "core/include/Application.h"
#include "core/include/ConfigNode.h"
#include "core/include/SubsystemMaker.h"

namespace ram {
namespace core {

Application::Application(std::string configPath)
{
    // NOTE:
    // Right now this is manual and hard coded, it will be done automatically
    // using dependenices and a topological sort based on subsystem dependencies
    
    core::ConfigNode root = core::ConfigNode::fromFile(configPath);
    core::ConfigNode sys_cfg(root["Subsystems"]);

    assert(sys_cfg.exists("Vehicle") && "Error vehicle not present in Config");
    core::SubsystemPtr vehicle(ram::core::SubsystemMaker::newObject(
         std::make_pair(sys_cfg["Vehicle"], SubsystemList()) ) );
    m_subsystems["Vehicle"] = vehicle;
    m_order.push_back("Vehicle");


    // Only do the controller if it exists
    if (sys_cfg.exists("Controller"))
    {
        SubsystemList deps;
        deps.push_back(vehicle);
        core::SubsystemPtr controller(ram::core::SubsystemMaker::newObject(
            std::make_pair(sys_cfg["Controller"], deps) ) );
        m_subsystems["Controller"] = controller;
        m_order.push_back("Controller");
    }

    BOOST_FOREACH(std::string name, m_order)
    {
        core::ConfigNode cfg(sys_cfg[name]);
        if (cfg.exists("update_interval"))
            m_subsystems[name]->background(cfg["update_interval"].asInt());
    }
}

Application::~Application()
{
    // TODO: Think more about this
    // We will force deletion here.  shared_ptr really should not be used here
    // because clients should not be able to influence destruction of subsystems
    // It is believed that the crashes which result from the use of a dead
    // pointer will be easier to debug than odd shared_ptr issues
    // I should consider a custom deleter that sets the pointer zero
    
    // Go through subsystems in the reverse order of contrustuction and shut
    // them down
    for (int i = m_order.size() - 1; i >= 0; --i)
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

std::vector<std::string> Application::getSubsystemName()
{
    return m_order;
}

} // namespace core
} // namespace ram

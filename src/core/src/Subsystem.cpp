/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Subsystem.cpp
 */

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/EventHub.h"

namespace ram {
namespace core {

Subsystem::~Subsystem()
{
}

std::string Subsystem::getName()
{
    return m_name;
}
    
Subsystem::Subsystem(std::string name, EventHubPtr eventHub) :
    EventPublisher(eventHub),
    m_name(name)
{
}

Subsystem::Subsystem(std::string name, SubsystemList list) :
    EventPublisher(getSubsystemOfExactType<EventHub>(list)),
    m_name(name)
{
}
    
} // namespace core
} // namespace ram
    

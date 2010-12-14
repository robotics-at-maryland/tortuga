/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/src/NetworkAdapter.cpp
 */

// Project Includes
#include "core/include/NetworkAdapter.h"
#include "core/include/SubsystemMaker.h"

RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::core::NetworkAdapter, NetworkAdapter);

namespace ram {
namespace core {

NetworkAdapter::NetworkAdapter(ConfigNode config, SubsystemList deps)
    : Subsystem(config["name"].asString("NetworkAdapter"), deps)
{
    m_ic = Ice::initialize();
    m_adapter = m_ic->createObjectAdapterWithEndpoints(
        getName(), config["endpoints"].asString("default -p 10000"));
}

NetworkAdapter::~NetworkAdapter()
{
    m_ic->destroy();
}

ObjectPrx NetworkAdapter::add(ObjectPtr object, std::string identity)
{
    return m_adapter->add(object, m_ic->stringToIdentity(identity));
}

void NetworkAdapter::remove(std::string identity)
{
    m_adapter->remove(m_ic->stringToIdentity(identity));
}

void NetworkAdapter::background(int interval)
{
    /* Activate network connections */
    m_adapter->activate();
}

void NetworkAdapter::unbackground(bool join)
{
    /* Deactivate network connections */
    m_adapter->deactivate();
}

} // namespace core
} // namespace ram

/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/src/NetworkAdapter.cpp
 */

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "network/include/NetworkAdapter.h"
#include "network/include/AdapterMaker.h"
#include "core/include/Exception.h"
#include "core/include/SubsystemMaker.h"

RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::network::NetworkAdapter, NetworkAdapter);

namespace ram {
namespace network {

NetworkAdapter::NetworkAdapter(core::ConfigNode config,
                               core::SubsystemList deps)
    : core::Subsystem(config["name"].asString("NetworkAdapter"), deps)
{
    m_ic = Ice::initialize();
    m_adapter = m_ic->createObjectAdapterWithEndpoints(
        getName(), config["endpoints"].asString("default -p 10000"));

    /* Create proxy for each "dependency" */
    BOOST_FOREACH(core::SubsystemPtr subsystem, deps)
    {
        try {
            Ice::ObjectPtr object = AdapterMaker::newObject(subsystem);
            m_adapter->add(object, m_ic->stringToIdentity(
                               subsystem->getName()));
        } catch (core::MakerNotFoundException &ex) {
            std::cerr << ex.what() << std::endl;
        }
    }

    m_adapter->activate();
}

NetworkAdapter::~NetworkAdapter()
{
    m_ic->destroy();
}

void NetworkAdapter::background(int interval)
{
}

void NetworkAdapter::unbackground(bool join)
{
}

} // namespace network
} // namespace ram

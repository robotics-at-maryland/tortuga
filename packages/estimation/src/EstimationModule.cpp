/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/EstimationModule.cpp
 */


// STD Include
#include <string.h>

// Library Includes

// Package Includes
#include "estimation/include/EstimationModule.h"
#include <boost/bind.hpp>

namespace ram {
namespace estimation {

EstimationModule::EstimationModule(core::EventHubPtr eventHub,
                                   std::string name, EstimatedStatePtr estState,
                                   core::Event::EventType type) :
    core::EventPublisher(eventHub, name),
    m_name(name),
    m_estimatedState(estState),
    m_connection(eventHub->subscribeToType(
                     type,
                     boost::bind(&ram::estimation::EstimationModule::update,
                                 this, _1)))
{
}

EstimationModule::~EstimationModule()
{
    m_connection->disconnect();
}

} // namespace estimation
} // namespace ram

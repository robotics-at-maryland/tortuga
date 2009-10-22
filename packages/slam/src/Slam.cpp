/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/slam/src/Slam.cpp
 */

// STD Libraries
#include <cassert>

// Library Includes
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/EventHub.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/EventConnection.h"
#include "vision/include/Events.h"

#include "slam/include/Slam.h"

// Register slam into the maker subsystem
//RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::slam::Slam, Slam);

namespace ram {
namespace slam {

Slam::Slam(core::ConfigNode config, core::SubsystemList deps) :
    ISlam(config["name"].asString(),
	  core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_controller(core::Subsystem::getSubsystemOfType<
		 control::IController>(deps)),
    m_vehicle(core::Subsystem::getSubsystemOfType<
	      vehicle::IVehicle>(deps))
{
    // Find all of the objects
    core::ConfigNode objectConfig = config["objects"];

    // They should be in a map with the object name and the default vector2
    core::NodeNameList objectList = config["objects"].subNodes();

    // Iterate over all of the objects and add them to the map
    core::NodeNameListIter beg = objectList.begin();
    core::NodeNameListIter end = objectList.end();
    
    for ( ; beg != end; ++beg) {
	double x = objectConfig[*beg][0].asDouble();
	double y = objectConfig[*beg][1].asDouble();
	m_objects[*beg] = math::Vector2(x, y);
    }

    // Subscribe to all of the vision events
    core::EventConnectionPtr connection = subscribe(
	vision::EventType::LIGHT_FOUND,
        boost::bind(&Slam::onLightUpdate, this, _1));
    m_connections.push_back(connection);
}

Slam::~Slam()
{
    BOOST_FOREACH(core::EventConnectionPtr event, m_connections) {
	event->disconnect();
    }
}

math::Vector2 Slam::getObjectPosition(std::string name)
{
    ObjectMapIter iter = m_objects.find(name);

    // Assert that this object must be found
    assert(iter != m_objects.end());
    return iter->second;
}

math::Vector2 Slam::getRelativePosition(std::string name)
{
    ObjectMapIter iter = m_objects.find(name);

    // Assert that this object must be found
    assert(iter != m_objects.end());
    return m_vehicle->getPosition() - iter->second;
}

bool Slam::hasObject(std::string name)
{
    // count() will return 1 if it exists, 0 if it doesn't
    return (m_objects.count(name) == 1) ? true : false;
}

void Slam::onLightUpdate(core::EventPtr event)
{
}

void Slam::onPipeUpdate(core::EventPtr event)
{
}

void Slam::onBwireUpdate(core::EventPtr event)
{
}

void Slam::onBinUpdate(core::EventPtr event)
{
}

void Slam::onTargetUpdate(core::EventPtr event)
{
}

} // namespace slam
} // namespace ram

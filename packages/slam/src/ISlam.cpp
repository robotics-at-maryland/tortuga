/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/slam/src/ISlam.cpp
 */

// Project Includes
#include "slam/include/ISlam.h"

RAM_CORE_EVENT_TYPE(ram::slam::ISlam, MAP_UPDATE);

namespace ram {
namespace slam {
    /*
typedef std::map<std::string,math::Vector2> ObjectPair;
typedef ObjectPair::iterator ObjectPairIter;

math::Vector2 ISlam::getObjectPosition(std::string name)
{
    // Find the object
    ObjectPairIter obj = m_objects.find(name);

    // Check if the object was found
    if (obj != m_objects.end()) {
    	return obj->second;
    } else {
	// Return the zero vector if no object was found
    	return math::Vector2::ZERO;
    }
}

math::Vector2 ISlam::getRelativePosition(std::string name)
{
    // Find the object
    ObjectPairIter obj = m_objects.find(name);
    if (obj != m_objects.end()) {
	// Take the objects location and find it relative to the vehicle's
	// current position
	math::Vector2 objLoc = obj->second;
	//return m_vehicle->getPosition() - objLoc;
	return objLoc;
    } else {
	// Return the zero vector if no object was found
	return math::Vector2::ZERO;
    }
    }*/

ISlam::ISlam(std::string name, core::EventHubPtr eventHub) :
    core::Subsystem(name, eventHub)
{
}

ISlam::~ISlam()
{
}

} // namespace slam
} // namespace ram

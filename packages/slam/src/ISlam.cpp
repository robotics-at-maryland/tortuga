/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/slam/src/ISlam.cpp
 */

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "slam/include/ISlam.h"

namespace ram {
namespace slam {

    //typedef map<std::string,math::Vector2> ObjectPair;
    //typedef ObjectPair::iterator ObjectPairIter;

    //math::Vector2 ISlam::getObjectLocation(std::string name)
    //{
    //ObjectPairIter obj = m_objects.find(name);
    //if (obj != m_objects.end()) {
    //	return obj->second;
    //} else {
    //	return Vector2(0, 0);
    //}
    //return Vector2(0, 0);
    //}

    //math::Vector2 ISlam::getRelativeLocation(std::string name)
    //{
    //return Vector2(0, 0);
    //}

//void ISlam::onLightUpdate(core::EventPtr event)
//{
// update light position
//}

ISlam::ISlam(std::string name, core::EventHubPtr eventHub) :
    core::Subsystem(name, eventHub)
{
    //core::EventConnectionPtr lightConnection = eventHub->subscribe(
    //    vision::LIGHT_FOUND, boost::bind(&ISlam::onLightUpdate, this, _1));
    //m_connections.push_back(lightConnection);
}

ISlam::~ISlam()
{
    //BOOST_FOREACH(core::EventConnectionPtr connection, m_connections)
    //{
    //connection->disconnect();
    //}
}

} // namespace slam
} // namespace ram

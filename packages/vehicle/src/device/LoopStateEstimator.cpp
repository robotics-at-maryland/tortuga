/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/LoopStateEstimator.cpp
 */

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "vehicle/include/device/LoopStateEstimator.h"
#include "math/include/Vector3.h"

namespace ram {
namespace vehicle {
namespace device {

LoopStateEstimator::LoopStateEstimator(core::ConfigNode config,
                                       core::EventHubPtr eventHub,
                                       IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IStateEstimator(eventHub, config["name"].asString()),
    m_orientation(math::Quaternion::IDENTITY),
    m_velocity(math::Vector2::ZERO),
    m_position(math::Vector2::ZERO),
    m_depth(0)
{
    if (config.exists("Objects")) {
	core::ConfigNode objCfg(config["Objects"]);
	
	BOOST_FOREACH(std::string name, objCfg.subNodes())
	{
	    // Grab the depth
	    m_objDepth[name] = objCfg[name][2].asDouble();

	    // Grab the position
	    m_objPosition[name] = math::Vector2(objCfg[name][0].asDouble(),
						objCfg[name][1].asDouble());

	    // Set the velocity to 0
	    m_objVelocity[name] = math::Vector2(0, 0);

	    // Grab the orientation
	    double angle = objCfg[name][3].asDouble();
	    m_objOrientation[name] = math::Quaternion(math::Degree(angle),
						      math::Vector3::UNIT_Z);
	}
    }
}
    
LoopStateEstimator::~LoopStateEstimator()
{
}

int LoopStateEstimator::orientationUpdate(math::Quaternion orientation,
					  double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_orientation = orientation;

    return StateFlag::ORIENTATION;
}

int LoopStateEstimator::velocityUpdate(math::Vector2 velocity,
					double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_velocity = velocity;

    return StateFlag::VEL;
}

int LoopStateEstimator::positionUpdate(math::Vector2 position,
				       double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_position = position;

    return StateFlag::POS;
}
    
int LoopStateEstimator::depthUpdate(double depth,
				    double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_depth = depth;

    return StateFlag::DEPTH;
}
    
math::Quaternion LoopStateEstimator::getOrientation(std::string obj)
{
    if (obj == "vehicle") {
	core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
	return m_orientation;
    } else {
	core::ReadWriteMutex::ScopedReadLock lock(m_objMutex);
	std::map<std::string, math::Quaternion>::iterator iter =
	    m_objOrientation.find(obj);
	assert(iter != m_objOrientation.end() &&
	       "Object does not exist in the map");
	return iter->second;
    }
}

math::Vector2 LoopStateEstimator::getVelocity(std::string obj)
{
    if (obj == "vehicle") {
	core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
	return m_velocity;
    } else {
	core::ReadWriteMutex::ScopedReadLock lock(m_objMutex);
	std::map<std::string, math::Vector2>::iterator iter =
	    m_objVelocity.find(obj);
	assert(iter != m_objVelocity.end() &&
	       "Object does not exist in the map");
	return iter->second;
    }
}

math::Vector2 LoopStateEstimator::getPosition(std::string obj)
{
    if (obj == "vehicle") {
	core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
	return m_position;
    } else {
	core::ReadWriteMutex::ScopedReadLock lock(m_objMutex);
	std::map<std::string, math::Vector2>::iterator iter =
	    m_objPosition.find(obj);
	assert(iter != m_objPosition.end() &&
	       "Object does not exist in the map");
	return iter->second;
    }
}
    
double LoopStateEstimator::getDepth(std::string obj)
{
    if (obj == "vehicle") {
	core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
	return m_depth;
    } else {
	core::ReadWriteMutex::ScopedReadLock lock(m_objMutex);
	std::map<std::string, double>::iterator iter =
	    m_objDepth.find(obj);
	assert(iter != m_objDepth.end() &&
	       "Object does not exist in the map");
	return iter->second;
    }
}

bool LoopStateEstimator::hasObject(std::string obj)
{
    if (obj == "vehicle")
	return true;
    else {
	// If the object is in the position list it is in every list
	core::ReadWriteMutex::ScopedReadLock lock(m_objMutex);
	size_t count = m_objPosition.count(obj);
	return count == 1;
    }
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

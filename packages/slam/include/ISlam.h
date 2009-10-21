/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/slam/include/ISlam.h
 */

#ifndef RAM_SLAM_ISLAM_10_17_2009
#define RAM_SLAM_ISLAM_10_17_2009

// STD Includes
#include <string>
#include <map>
#include <vector>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/Event.h"
#include "core/include/Subsystem.h"
#include "math/include/Vector2.h"
#include "vehicle/include/IVehicle.h"

#include "slam/include/Common.h"

// Must Be Included last
#include "slam/include/Export.h"

namespace ram {
namespace slam {

class ISlam;
typedef boost::shared_ptr<ISlam> ISlamPtr;

/** A single interface for the SLAM algorithm.
 *
 *  This ensures that every SLAM instance acts within the system
 *  in the desired manner.
 */

class RAM_EXPORT ISlam : public core::Subsystem
{
public:
    virtual ~ISlam();

    static const core::Event::EventType UPDATE;

    //math::Vector2 getObjectPosition(std::string name);

    //math::Vector2 getRelativePosition(std::string name);

protected:
    ISlam(std::string name,
	  core::EventHubPtr eventHub = core::EventHubPtr());

    //void onLightUpdate(core::EventPtr event);

    std::map<std::string, math::Vector2> m_objects;

    //vehicle::IVehicle m_vehicle;

    //std::vector<core::EventConnectionPtr> m_connections;
};

} // namespace slam
} // namespace ram

#endif // RAM_SLAM_ISLAM_10_17_2009

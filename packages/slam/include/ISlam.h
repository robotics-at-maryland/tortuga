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
#include <vector>

// Project Includes
#include "core/include/Event.h"
#include "core/include/Subsystem.h"
#include "math/include/Vector2.h"

#include "slam/include/Common.h"

// Must Be Included last
#include "slam/include/Export.h"

namespace ram {
namespace slam {

/** A single interface for the SLAM algorithm.
 *
 *  This ensures that every SLAM instance acts within the system
 *  in the desired manner.
 */

class RAM_EXPORT ISlam : public core::Subsystem
{
public:
    virtual ~ISlam() {};

    /** When the SLAM algorithm's map is updated.
     *
     *  Type is ram::slam::MapUpdateEvent;
     */
    static const core::Event::EventType MAP_UPDATE;

    /** Position of 'name' object in the SLAM algorithm */
    virtual math::Vector2 getObjectPosition(std::string name) = 0;

    /** Position of 'name' object relative to the vehicle */
    virtual math::Vector2 getRelativePosition(std::string name) = 0;

    /** Whether object 'name' is being tracked */
    virtual bool hasObject(std::string name) = 0;

protected:
    ISlam(std::string name,
	  core::EventHubPtr eventHub = core::EventHubPtr());
};

} // namespace slam
} // namespace ram

#endif // RAM_SLAM_ISLAM_10_17_2009

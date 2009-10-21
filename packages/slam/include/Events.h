/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/slam/include/Events.h
 */

#ifndef RAM_SLAM_EVENTS_10_21_2009
#define RAM_SLAM_EVENTS_10_21_2009

// STD Includes
#include <string>
#include <vector>

// Project Includes
#include "core/include/Event.h"
#include "math/include/Vector2.h"

namespace ram {
namespace slam {

struct MapUpdateEvent : public core::Event
{
    std::string name;

    math::Vector2 oldLoc;

    math::Vector2 newLoc;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<MapUpdateEvent> MapUpdateEventPtr;

} // namespace slam
} // namespace ram

#endif // RAM_SLAM_EVENTS_10_21_2009

/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jsternbe@umd.edu>
 * File:  packages/slam/src/Events.cpp
 */

// Project Includes
#include "core/include/Feature.h"
#include "slam/include/Events.h"

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS) && defined(RAM_WITH_CORE)

#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::slam::MapUpdateEvent>
RAM_SLAM_MAPUPDATE;

#endif // RAM_WITH_WRAPPERS

namespace ram {
namespace slam {

core::EventPtr MapUpdateEvent::clone()
{
    MapUpdateEventPtr event = MapUpdateEventPtr(new MapUpdateEvent());
    copyInto(event);
    event->name = name;
    event->oldLoc = oldLoc;
    event->newLoc = newLoc;
    return event;
}
    
} // namespace slam
} // namespace ram

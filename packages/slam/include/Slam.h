/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/slam/include/Slam.h
 */

#ifndef RAM_SLAM_SLAM_10_21_2009
#define RAM_SLAM_SLAM_10_21_2009

// STD Includes
#include <string>
#include <map>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/Subsystem.h"
#include "math/include/Vector2.h"
#include "control/include/IController.h"
#include "vehicle/include/IVehicle.h"

#include "slam/include/ISlam.h"

namespace ram {
namespace slam {

typedef std::map<std::string, math::Vector2> ObjectMap;
typedef ObjectMap::iterator ObjectMapIter;

class RAM_EXPORT Slam : public ISlam
{
public:
    Slam(core::ConfigNode config,
	 core::SubsystemList deps = core::SubsystemList());

    virtual ~Slam();

    virtual math::Vector2 getObjectPosition(std::string name);

    virtual math::Vector2 getRelativePosition(std::string name);

    virtual bool hasObject(std::string name);

protected:
    /** Is called on every LIGHT_FOUND event */
    virtual void onLightUpdate(core::EventPtr event);

    /** Is called on every PIPE_FOUND event */
    virtual void onPipeUpdate(core::EventPtr event);
    
    /** Is called on every BARBED_WIRE_FOUND event */
    virtual void onBwireUpdate(core::EventPtr event);

    /** Is called on every BIN_FOUND event */
    virtual void onBinUpdate(core::EventPtr event);

    /** Is called on every TARGET_FOUND event */
    virtual void onTargetUpdate(core::EventPtr event);

    ObjectMap m_objects;

    control::IControllerPtr m_controller;

    vehicle::IVehiclePtr m_vehicle;

    std::vector<core::EventConnectionPtr> m_connections;
};

} // namespace slam
} // namespace ram

#endif // RAM_SLAM_SLAM_10_21_2009

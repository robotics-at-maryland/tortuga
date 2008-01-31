/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/VisionSystem.h
 */

#ifndef RAM_VISION_VISIONSYSTEM_H_01_29_2008
#define RAM_VISION_VISIONSYSTEM_H_01_29_2008

// Project Includes
#include "core/include/Subsystem.h"
#include "vision/include/VisionRunner.h"

namespace ram {
namespace vision {

class VisionSystem : public core::Subsystem
{
public:
    VisionSystem(std::string name,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    
    VisionSystem(std::string name, core::SubsystemList list);
    
    void redLightDetectorOn();
    
    void redLightDetectorOff();

private:
    VisionRunner vr;
    
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_VISIONSYSTEM_H_01_29_2008

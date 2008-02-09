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
#include "core/include/ConfigNode.h"

#include "vision/include/Common.h"

// Must Be Included Last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class VisionRunner;

class RAM_EXPORT VisionSystem : public core::Subsystem
{
public:
    VisionSystem(core::ConfigNode config, core::SubsystemList deps);
    VisionSystem(Camera* forward, Camera* downward, core::SubsystemList list);
    
    virtual ~VisionSystem();
    
    void redLightDetectorOn();
    
    void redLightDetectorOff();
    virtual void background(int interval) {
//        Updatable::background(interval);
    };
    virtual void unbackground(bool join = false) {
//        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
      return false;
//        return Updatable::backgrounded();
    };

    /** Grabs latest vehicle state, runs the controller, the commands the
        thrusters */
    virtual void update(double timestep)
    {
    };

private:
    /** Initializes all internal members */
    void init();
    
    Camera* m_forwardCamera;
    Camera* m_downwardCamera;
    VisionRunner* m_forward;
    VisionRunner* m_downward;

    DetectorPtr m_redLightDetector;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_VISIONSYSTEM_H_01_29_2008

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

// Must Be Included Last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class VisionRunner;

class RAM_EXPORT VisionSystem : public core::Subsystem
{
public:
    VisionSystem(ram::core::ConfigNode config, ram::core::SubsystemList deps);
    VisionSystem(std::string name,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    
    VisionSystem(std::string name, core::SubsystemList list);
    
    virtual ~VisionSystem();
    
    void redLightDetectorOn();
    void redLightDetectorOff();
    void binDetectorOn();
    void binDetectorOff();
    void pipeLineDetectorOn();
    void pipeLineDetectorOff();
    void gateDetectorOn();
    void gateDetectorOff();
    
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
    VisionRunner* vr;
    
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_VISIONSYSTEM_H_01_29_2008

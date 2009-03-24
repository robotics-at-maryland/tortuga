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
    VisionSystem(CameraPtr forward, CameraPtr downward,
                 core::ConfigNode config, core::SubsystemList deps);
    
    virtual ~VisionSystem();
    
    void redLightDetectorOn();
    void redLightDetectorOff();
    void binDetectorOn();
    void binDetectorOff();
    void pipeLineDetectorOn();
    void pipeLineDetectorOff();
    void ductDetectorOn();
    void ductDetectorOff();
    void downwardSafeDetectorOn();
    void downwardSafeDetectorOff();
    void gateDetectorOn();
    void gateDetectorOff();
    void targetDetectorOn();
    void targetDetectorOff();
    void barbedWireDetectorOn();
    void barbedWireDetectorOff();

    /** Calls setPriority on Cameras, VisionRunners, and Recorders
     *
     *  @note Only valid when in testing mode, set by 'testing' in config file
     */
    virtual void setPriority(core::IUpdatable::Priority priority);

    /** Not implemented, always returns core::IUpdatable::NORMAL_PRIORITY */
    virtual core::IUpdatable::Priority getPriority() {
        return core::IUpdatable::NORMAL_PRIORITY;
    }

    /** Not implemented */
    virtual void setAffinity(size_t) {};
    /** Not implemented */
    virtual int getAffinity() {
        return -1;
    };
    
    /** Calls background on the Cameras, VisionRunners and Recorders
     *
     *  @note Only valid when in testing mode, set by 'testing' in config file
     */
    virtual void background(int interval);

    /** Calls unbackgrond on the Cameras, VisionRunners, and Recorders
     *
     *  @note Only valid when in testing mode, set by 'testing' in config file
     */
    virtual void unbackground(bool join = false);

    /** Not implemented, always return true */
    virtual bool backgrounded() {
      return true;
//        return Updatable::backgrounded();
    };

    /** Updates Cameras, then VisionRunners, then Recorders
     *
     *  @note Only valid when in testing mode, set by 'testing' in config file
     */
    virtual void update(double timestep);

private:
    /** Initializes all internal members */
    void init(core::ConfigNode config, core::EventHubPtr eventHub);
    
    CameraPtr m_forwardCamera;
    CameraPtr m_downwardCamera;

    Recorder* m_forwardFileRecorder;
    Recorder* m_downwardFileRecorder;
    Recorder* m_forwardNetworkRecorder;
    Recorder* m_downwardNetworkRecorder;
    
    VisionRunner* m_forward;
    VisionRunner* m_downward;

    DetectorPtr m_redLightDetector;
    DetectorPtr m_binDetector;
    DetectorPtr m_pipelineDetector;
    DetectorPtr m_ductDetector;
    DetectorPtr m_downwardSafeDetector;
    DetectorPtr m_gateDetector;
    DetectorPtr m_targetDetector;
    DetectorPtr m_barbedWireDetector;

    /** Flag which when true enables use of back/unback and update */
    bool m_testing;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_VISIONSYSTEM_H_01_29_2008

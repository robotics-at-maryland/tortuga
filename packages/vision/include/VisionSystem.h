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

// STD Includes
#include <map>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/ConfigNode.h"
#include "core/include/Forward.h"

#include "vision/include/Common.h"

#include "math/include/Math.h"

// Must Be Included Last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class VisionRunner;
class VisionSystem;

typedef boost::shared_ptr<VisionSystem> VisionSystemPtr;

class RAM_EXPORT VisionSystem : public core::Subsystem
{
public:
    VisionSystem(core::ConfigNode config, core::SubsystemList deps);
    VisionSystem(CameraPtr forward, CameraPtr downward,
                 core::ConfigNode config, core::SubsystemList deps);
    
    virtual ~VisionSystem();
    
    virtual void buoyDetectorOn();
    virtual void buoyDetectorOff();
    virtual void binDetectorOn();
    virtual void binDetectorOff();
    virtual void pipeLineDetectorOn();
    virtual void pipeLineDetectorOff();
    virtual void downwardSafeDetectorOn();
    virtual void downwardSafeDetectorOff();
    virtual void gateDetectorOn();
    virtual void gateDetectorOff();
    virtual void cupidDetectorOn();
    virtual void cupidDetectorOff();
    virtual void loversLaneDetectorOn();
    virtual void loversLaneDetectorOff();
    virtual void monoPipeLineDetectorOn();
    virtual void monoPipeLineDetectorOff();


    /** Creates a recorder based on the string with the given policy
     *
     *  @param recorderString
     *      A string of the same format that is passed to
     *      Recorder::createRecorderFromString.
     *  @param policy
     *      -1 if you want to record as many frames as possible, otherwise its
     *      the max frame rate you wish to record at.
     */
    void addForwardRecorder(std::string recorderString, int policy,
                            bool debugPrint = false);
    void addDownwardRecorder(std::string recorderString, int policy,
                             bool debugPrint = false);
    
    /** Removes the recorder, shutting it down
     *
     *  @param recorderString
     *      Must be the same string that was passed to "addRecorder"
     */
    void removeForwardRecorder(std::string recorderString);
    void removeDownwardRecorder(std::string recorderString);
    
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

    /** Get horizontal field of view */
    static math::Degree getFrontHorizontalFieldOfView();

    /** Get vertical field of view */
    static math::Degree getFrontVerticalFieldOfView();

    /** Get horizontal pixel resolution */
    static int getFrontHorizontalPixelResolution();

    /** Get vertical pixel resolution */
    static int getFrontVerticalPixelResolution();

    /** Get horizontal field of view */
    static math::Degree getDownHorizontalFieldOfView();

    /** Get vertical field of view */
    static math::Degree getDownVerticalFieldOfView();

    /** Get horizontal pixel resolution */
    static int getDownHorizontalPixelResolution();

    /** Get vertical pixel resolution */
    static int getDownVerticalPixelResolution();

    /** Set horizontal field of view (Internal use only) */
    static void _setFrontHorizontalFieldOfView(math::Degree degree);

    /** Set vertical field of view (Internal use only) */
    static void _setFrontVerticalFieldOfView(math::Degree degree);

    /** Set horizontal pixel resolution (Internal use only) */
    static void _setFrontHorizontalPixelResolution(int pixels);

    /** Set vertical pixel resolution (Internal use only) */
    static void _setFrontVerticalPixelResolution(int pixels);

    /** Set horizontal field of view (Internal use only) */
    static void _setDownHorizontalFieldOfView(math::Degree degree);

    /** Set vertical field of view (Internal use only) */
    static void _setDownVerticalFieldOfView(math::Degree degree);

    /** Set horizontal pixel resolution (Internal use only) */
    static void _setDownHorizontalPixelResolution(int pixels);

    /** Set vertical pixel resolution (Internal use only) */
    static void _setDownVerticalPixelResolution(int pixels);

    /** Attempts to find the vision system configuration section */
    static core::ConfigNode findVisionSystemConfig(core::ConfigNode cfg,
                                                   std::string& nodeUsed);

private:
    /** Initializes all internal members */
    void init(core::ConfigNode config, core::EventHubPtr eventHub);

    void createRecordersFromConfig(core::ConfigNode recorderCfg,
                                   CameraPtr camera);

    void createRecorder(CameraPtr camera, std::string recorderString,
                        int frameRate, bool debugPrint = false);

    /** Add a detector to the forward runner */
    void addForwardDetector(DetectorPtr detector);

    /** Add a detector to the downward runner */
    void addDownwardDetector(DetectorPtr detector);

    core::ConfigNode getConfig(core::ConfigNode config, std::string name);
    
    CameraPtr m_forwardCamera;
    CameraPtr m_downwardCamera;

    typedef std::map<std::string, Recorder*> StrRecorderMap;
    typedef StrRecorderMap::value_type StrRecorderMapPair;
    StrRecorderMap m_recorders;
    
    VisionRunner* m_forward;
    VisionRunner* m_downward;

    DetectorPtr m_buoyDetector;
    DetectorPtr m_binDetector;
    DetectorPtr m_pipelineDetector;
    DetectorPtr m_downwardSafeDetector;
    DetectorPtr m_gateDetector;
    DetectorPtr m_cupidDetector;
    DetectorPtr m_loversLaneDetector;
    DetectorPtr m_monoPipeDetector;

    /** Flag which when true enables use of back/unback and update */
    bool m_testing;

    static math::Degree s_frontHorizontalFieldOfView;
    static math::Degree s_frontVeritcalFieldOfView;
    static int s_frontHorizontalPixelResolution;
    static int s_frontVerticalPixelResolution;
    static math::Degree s_downHorizontalFieldOfView;
    static math::Degree s_downVeritcalFieldOfView;
    static int s_downHorizontalPixelResolution;
    static int s_downVerticalPixelResolution;

};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_VISIONSYSTEM_H_01_29_2008

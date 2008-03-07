/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/VisionRunner.h
 */

#ifndef RAM_VISION_RUNNER_H_07_11_2007
#define RAM_VISION_RUNNER_H_07_11_2007

// STD Includes
#include <set>
//#include <utility>

// Project Includes
#include "vision/include/Events.h"
#include "vision/include/Common.h"
#include "vision/include/Recorder.h"

#include "core/include/Event.h"
#include "core/include/ThreadedQueue.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram { 
namespace vision {

/** Runs a set of detectors on a specific camera
 *
 *  If the runner has detecctors, and the given camera is caputring images the
 *  detectors will be running.
 */
class RAM_EXPORT VisionRunner : public Recorder
{
public:
    /** Constructor
     *
     *  @param camera  The camera to record images from
     *  @param policy  Determines how often images from the camera are recorded
     *  @param policyArg  An argument for use by the given recording policy.
     *
     */
    VisionRunner(Camera* camera, Recorder::RecordingPolicy policy,
                 int policyArg = 0);
    ~VisionRunner();
    
    /** Process detector changes, then goes into the normal Recorder update */
    virtual void update(double timestep);
    
    /** Adds the given detector to the list running detectors */
    void addDetector(DetectorPtr detector);

    /** Removes the given detector from the list of running detectors */
    void removeDetector(DetectorPtr detector);

    /** Removes all detectors */
    void removeAllDetectors();
    
protected:
    /** Waits for 1/30 of second, then just keeps looping */
    virtual void waitForImage(Camera* camera);
    
    /** Called when we have a new image process */
    virtual void recordFrame(Image* image);
    
private:
    enum ChangeType {
        ADD,
        REMOVE,
        REMOVE_ALL
    };
    typedef std::pair<ChangeType, DetectorPtr> DetectorChange;

    /** Runs through the detectorChanges queue and adds/remove detectors
     *
     *  @param   canBackground  Allows the function to change the backgrounded
     *                          state if needed.
     *
     *  @return  Whether or not the background thread needs to be toggle on/off
     */
    bool processDetectorChanges(bool canBackground = true);
    
    /** Detectors to be added or removed */
    core::ThreadedQueue<DetectorChange> m_detectorChanges;

    /** Current list of dectors being added */
    std::set<DetectorPtr> m_detectors;
};
        
} // namespace vision
} // namespace ram

#endif

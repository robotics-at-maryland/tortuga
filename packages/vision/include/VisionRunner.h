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

#include "core/include/Event.h"
#include "core/include/Updatable.h"
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
class RAM_EXPORT VisionRunner : public core::Updatable
{
public:
    VisionRunner(Camera* camera);
    ~VisionRunner();
    
    /** Waits for the next camera image then runs the detectors on the image */
    virtual void update(double timestep);
    
    /** Adds the given detector to the list running detectors */
    void addDetector(DetectorPtr detector);

    /** Removes the given detector from the list of running detectors */
    void removeDetector(DetectorPtr detector);

    /** Removes all detectors */
    void removeAllDetectors();
    
    /** Stops the background process thread */
    virtual void unbackground(bool join = false);
    
private:
    enum ChangeType {
        ADD,
        REMOVE,
        REMOVE_ALL,
        STOP
    };
    typedef std::pair<ChangeType, DetectorPtr> DetectorChange;

    /** Runs through the detectorChanges queue and adds/remove detectors
     *
     *  @return  true if the we get a STOP processing change
     */
    bool processDetectorChanges();

    /** Processes and indivual changes to the detector list
     *
     *  @return  true if the we get a STOP processing change
     */
    bool processDetectorChange(DetectorChange& change);
    
    /** Detectors to be added or removed */
    core::ThreadedQueue<DetectorChange> m_detectorChanges;

    /** Current list of dectors being added */
    std::set<DetectorPtr> m_detectors;

    /** Camera to grabe images from */
    Camera* m_camera;

    /** Image we capture from the camera */
    Image* m_image;
};
        
} // namespace vision
} // namespace ram

#endif

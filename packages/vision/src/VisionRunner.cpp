/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/VisionRunner.h
 */

// STD Includes
#include <utility>

// Project Includes
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/VisionRunner.h"
#include "vision/include/Camera.h"
#include "vision/include/Detector.h"

namespace ram {
namespace vision {

VisionRunner::VisionRunner(Camera* camera, Recorder::RecordingPolicy policy,
                           int policyArg) :
    Recorder(camera, policy, policyArg)
{
}

VisionRunner::~VisionRunner()
{
    cleanUp();
    
    // stop background thread, and wait till it joins
    Updatable::unbackground(true);
}
    
void VisionRunner::update(double timestep)
{
    // Process changes to the detector changes
    bool toggleBackground = processDetectorChanges(false);

    // Normal update (calls record frame as needed) only if backgrounded
    Recorder::update(timestep);

    if (toggleBackground)
    {
        if (backgrounded())
            unbackground(true);
        else
            background(-1);
    }
        
}
    
void VisionRunner::addDetector(DetectorPtr detector)
{
    m_detectorChanges.push(std::make_pair(ADD, detector));

    // Make change right away if there is no background thread
    if (!backgrounded())
        processDetectorChanges();
}

void VisionRunner::removeDetector(DetectorPtr detector)
{
    m_detectorChanges.push(std::make_pair(REMOVE, detector));

    // Make change right away if there is not background thread
    if (!backgrounded())
        processDetectorChanges();
}

void VisionRunner::removeAllDetectors()
{
    m_detectorChanges.push(std::make_pair(REMOVE_ALL, DetectorPtr()));

    // Make change right away if there is not background thread
    if (!backgrounded())
        processDetectorChanges();
}

void VisionRunner::recordFrame(Image* image)
{
    // Make any changes to the detectors which might have happend while
    // we waited for the image to appear
    if(processDetectorChanges() || (m_detectors.size() == 0))
        return;

    // Have each detector process the image
    BOOST_FOREACH(DetectorPtr detector, m_detectors)
    {
        detector->processImage(image);
    }
}
    
void VisionRunner::waitForImage(Camera* camera)
{
    boost::xtime timeout = {0, (int)(1e6 * 1000/33)}; // 33 milliseconds
    camera->waitForImage(0, timeout);
}
    
    
bool VisionRunner::processDetectorChanges(bool canBackground)
{
    size_t startSize = m_detectors.size();
    DetectorChange change;
    
    while(m_detectorChanges.popNoWait(change))
    {
        switch(change.first)
        {
            case ADD:
            {
                m_detectors.insert(change.second);
            }
            break;

            case REMOVE:
            {
                std::set<DetectorPtr>::iterator iter =
                    m_detectors.find(change.second);
                
                if (m_detectors.end() != iter)
                    m_detectors.erase(iter);
            }
            break;
            
            case REMOVE_ALL:
            {
                m_detectors.clear();
            }
            break;
        }
    }

    // Background or unbackground depending on size change
    if ((0 == startSize) && (m_detectors.size() > 0))
    {
        if (canBackground)
            background(-1);
        return true;
    }
    else if ((startSize > 0) && (0 == m_detectors.size()))
    {
        if (canBackground)
            Updatable::unbackground(true);
        return true;
    }

    return false;
}
    
} // namespace vision
} // namespace ram


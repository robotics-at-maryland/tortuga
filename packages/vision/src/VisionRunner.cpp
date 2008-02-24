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
#include "vision/include/OpenCVImage.h"
#include "vision/include/Detector.h"

namespace ram {
namespace vision {

VisionRunner::VisionRunner(Camera* camera) :
    m_camera(camera),
    m_image(new OpenCVImage(camera->width(), camera->height()))
{
}

VisionRunner::~VisionRunner()
{
    // stop background thread, and wait till it joins
    Updatable::unbackground(true);

    delete m_image;
}
    
void VisionRunner::update(double timestep)
{
    // Process changes to the detector list (return if we get a STOP change)
    if(processDetectorChanges())
        return;

    // Wait for the next image
    boost::xtime timeout = {0, (int)(1e6 * 1000/33)}; // 33 milliseconds
    if(m_camera->waitForImage(m_image, timeout))
    {
        // Make any changes to the detectors which might have happend while
        // we waited for the image to appear
        if(processDetectorChanges() || (m_detectors.size() == 0))
            return;

        // Have each detector process the image
        BOOST_FOREACH(DetectorPtr detector, m_detectors)
        {
            detector->processImage(m_image);
        }
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

void VisionRunner::unbackground(bool join)
{
    // Stop the detector wait loop if there is one
    m_detectorChanges.push(std::make_pair(STOP, DetectorPtr()));
    
    Updatable::unbackground(join);
}
    
bool VisionRunner::processDetectorChanges()
{
    bool stopped = false;
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
            
            case STOP:
            {
                stopped = false;
            }
            break;
        }
    }

    // Background or unbackground depending on size change
    if ((0 == startSize) && (m_detectors.size() > 0))
        background(-1);
    else if ((startSize > 0) && (0 == m_detectors.size()))
        Updatable::unbackground(false);

   return stopped;
}

bool VisionRunner::processDetectorChange(DetectorChange& change)
{


    return false;
}
    
} // namespace vision
} // namespace ram


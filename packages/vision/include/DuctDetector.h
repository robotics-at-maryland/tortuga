/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Chris Giles
 * All rights reserved.
 *
 * Author: Chris Giles <cgiles17@umd.edu>
 * File:  packages/vision/include/DuctDetector.h
 */

#ifndef RAM_DUCT_DETECTOR_H_07_03_2008
#define RAM_DUCT_DETECTOR_H_07_03_2008

#include "cv.h"
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"

#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT DuctDetector : public Detector
{
public:
    DuctDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    DuctDetector(core::EventHubPtr eventHub = core::EventHubPtr());
        virtual ~DuctDetector();
        
    void processImage(Image* input, Image* output = 0);

    /** Normalized from -1 to 1*/
    double getX();
    
    /** Normalized from -1 to 1*/
    double getY();
    
    /** 0 at the closest 1 at the father */
    double getRange();
    
    double getRotation();
    
    bool getVisible();
    bool getAligned();
    
private:
    /** Stores the image we are currently working with */
    Image* m_working;
    double m_x, m_y, m_rotation, m_range;
    double n_x, n_y;
};

} // namespace vision
} // namespace ram

#endif // RAM_DUCT_DETECTOR_H_07_03_2008

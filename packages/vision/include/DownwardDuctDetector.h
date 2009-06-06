/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim17@umd.edu>
 * File:  packages/vision/include/DownwardDuctDetector.h
 */

#ifndef RAM_DOWNWARD_DUCT_DETECTOR_H_07_21_2008
#define RAM_DOWNWARD_DUCT_DETECTOR_H_07_21_2008

#include "cv.h"
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"

#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT DownwardDuctDetector : public Detector
{
public:
    DownwardDuctDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    DownwardDuctDetector(core::EventHubPtr eventHub = core::EventHubPtr());
	virtual ~DownwardDuctDetector();
        
    void processImage(Image* input, Image* output = 0);
    
private:
    void init(core::ConfigNode config);
    int yellow(unsigned char r, unsigned char g, unsigned char b, double minRedOverGreen, double maxRedOverGreen, double minRedOverBlue, double minGreenOverBlueOnFailure, int maxRedFailureThresh, int minTotalRGB, int minTotalRGBOnFailure);
    Image* m_working;
    double m_x, m_y;
    bool m_found;
    
    // Configurable variables
    double m_minRedOverGreen;
    double m_maxRedOverGreen;
    double m_minRedOverBlue;
    double m_minGreenOverBlueOnRedFailureForInsideDuct;

    int m_maxRedFailureThresh; //default 50
    int m_minTotalRGB;         //default 125
    int m_minTotalRGBOnFailure;//Default 150
    
};

} // namespace vision
} // namespace ram

#endif // RAM_DUCT_DETECTOR_H_07_03_2008


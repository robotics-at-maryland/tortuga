/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/OrangePipeDetector.h
 */

#ifndef RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007
#define RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007


// Project Includes
#include "vision/include/Common.h"
#include "vision/include/PipeDetector.h"
#include "core/include/ConfigNode.h"
#include "math/include/Math.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
        
class RAM_EXPORT OrangePipeDetector  : public PipeDetector
{
  public:
    OrangePipeDetector(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());
    ~OrangePipeDetector();
    
    void processImage(Image* input, Image* output= 0);
    
    bool found();

    /** Get normalized X cordinate of the center of the orange line */
    double getX();

    /** Get normalized Y cordinate of the center of the orange line */
    double getY();

    math::Degree getAngle();
    
  private:
    void init(core::ConfigNode config);

    /** Use Dan's custom mask_orange function */
    void filterForOrangeOld(Image* image);

    /** Use the color filter to filter for orange */
    //    void filterForOrange();

    /** Angle of the pipe */
    math::Radian m_angle;
    
    /** X cordinate of pipe */
    double m_lineX;
    
    /** Y cordinate of pipe */
    double m_lineY;

    double m_rOverGMin;
    double m_rOverGMax;
    double m_bOverRMax;
    bool m_found;

    /** Maximum distance for the pipe to be considred "centered" */
    double m_centeredLimit;

    /** Whether or not we are centered */
    bool m_centered;

    /** The minimum brightness a pixel has to be to be considered orange */
    int m_minBrightness;

    /** Number of times to erode the masked image before the hough */
    int m_erodeIterations;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007


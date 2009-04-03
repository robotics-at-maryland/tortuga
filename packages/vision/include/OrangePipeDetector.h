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
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "core/include/ConfigNode.h"
#include "math/include/Math.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
        
class RAM_EXPORT OrangePipeDetector  : public Detector
{
  public:
    OrangePipeDetector(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());
    OrangePipeDetector(Camera*);
    ~OrangePipeDetector();
    
    void processImage(Image* input, Image* output= 0);
    
    bool found();
    /** Get normalized X cordinate of the center of the orange line */
    double getX();
    /** Get normalized Y cordinate of the center of the orange line */
    double getY();
    math::Degree getAngle();
    void update();
    
    /** Turns on and off the use of the hough detector for angle */
    void setHough(bool value);
    void show(char* window);
    IplImage* getAnalyzedImage();
    
    
  private:
    void init(core::ConfigNode config);

    /** Determines the angle of the pipe inside the given blob 
     *  
     *  @return The angle of the pipe in radians (will have 90 degs added to 
     *          it).  It will return HOUGH_ERROR if it didn't find anything.
     */
    double findPipeAngle(BlobDetector::Blob pipeBlob, IplImage* image,
			 bool debug = false);

    /** Angle of the pipe */
    math::Radian m_angle;
    
    /** X cordinate of pipe */
    double m_lineX;
    
    /** Y cordinate of pipe */
    double m_lineY;
    IplImage* m_rotated;
    Image* m_frame;
    Camera* m_cam;

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

    /** Disabled use of hough */
    bool m_noHough;

    /** Finds the pipe blobs */
    BlobDetector m_blobDetector;

    int m_minPixels;
    int m_minPixelsFound;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007


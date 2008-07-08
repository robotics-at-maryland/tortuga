/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/RedLightDetector.h
 */

#ifndef RAM_RED_LIGHT_DETECTOR_H_06_23_2007
#define RAM_RED_LIGHT_DETECTOR_H_06_23_2007

// Library Includes
#include "cv.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT RedLightDetector : public Detector
{
  public:
    RedLightDetector(core::ConfigNode config,
                     core::EventHubPtr eventHub = core::EventHubPtr());
    RedLightDetector(Camera* camera);
    ~RedLightDetector();

    void update();
    void processImage(Image* input, Image* output= 0);

    bool found;

    /** Center of the red light in the local horizontal, -4/3 to 4/3 */
    double getX();
    
    /** Center of the red light in the local vertical, -1 to 1 */
    double getY();
    
    void show(char* window);
    IplImage* getAnalyzedImage();
    
  private:
    void init(core::ConfigNode config);

    // Process current state, and publishes LIGHT_FOUND event
    void publishFoundEvent(double lightPixelRadius);

    /** Processes the list of all found blobs and finds the larget valid one */
    bool processBlobs(const BlobDetector::BlobList& blobs,
                      BlobDetector::Blob& outBlob);
    
    int lightFramesOff;
    int lightFramesOn;
    int blinks;
    int spooky;
    bool startCounting;
    double m_redLightCenterX;
    double m_redLightCenterY;
    int minRedPixels;
    IplImage* image;
    IplImage* raw;
    IplImage* flashFrame;
    IplImage* saveFrame;
    CvPoint lightCenter;
    
    Image* frame;
    Camera* cam;

    /** Finds the red light */
    BlobDetector m_blobDetector;

    /** Initial level of minimum red pixels (and lower bound) */
    int m_initialMinRedPixels;

    /** Factor to change minRedPixels by for each found frame */
    double m_foundMinPixelScale;

    /** Factor to change minRedPixels by for each found frame */
    double m_lostMinPixelScale;

    /** Threshold for almost hitting the red light */
    double m_almostHitPercentage;

    /** How un square are blob can be and still be considered a red light */
    double m_maxAspectRatio;
    
    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;

    /** The threshold of the percentage of red in the image */
    double m_redPercentage;

    /** The threshold of the intensity of red in the image */
    int m_redIntensity;

    

    /** The ammout the aspect ratio of the bounding box can be non-square */
    //double m_aspectRatioDeviation;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_RED_LIGHT_DETECTOR_H_06_23_2007

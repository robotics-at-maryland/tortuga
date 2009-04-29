/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/TargetDetector.h
 */

#ifndef RAM_TARGET_DETECTOR_H_04_29_2009
#define RAM_TARGET_DETECTOR_H_04_29_2009

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT TargetDetector : public Detector
{
  public:
    TargetDetector(core::ConfigNode config,
                   core::EventHubPtr eventHub = core::EventHubPtr());
    ~TargetDetector();

    void processImage(Image* input, Image* output= 0);

    bool found();

    /** Center of the red light in the local horizontal, -4/3 to 4/3 */
    double getX();
    
    /** Center of the red light in the local vertical, -1 to 1 */
    double getY();

    /** The percent of the top of the image blacked out */
    void setTopRemovePercentage(double percent);

    /** The percent of the bottom of the image blacked out */
    void setBottomRemovePercentage(double percent);
        
  private:
    void init(core::ConfigNode config);

    /** Processes the list of all found blobs and finds the larget valid one */
    bool processBlobs(const BlobDetector::BlobList& blobs,
                      BlobDetector::Blob& outBlob);

    /** Filters for the green in the water */
    ColorFilter* m_filter;

    /** Working scratch image */
    Image* m_image;

    bool m_found;
    
    double m_targetCenterX;

    double m_targetCenterY;

    /** Finds the red light */
    BlobDetector m_blobDetector;

    /** How un square are blob can be and still be considered a target */
    double m_maxAspectRatio;
    
    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;

    /** Percentage of the image to remove from the bottom */
    double m_bottomRemovePercentage;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_TARGET_DETECTOR_H_04_29_2009

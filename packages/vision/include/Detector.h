/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Detector.h
 */

#ifndef RAM_VISION_DETECTOR_H_02_07_2008
#define RAM_VISION_DETECTOR_H_02_07_2008

// Project Includes
#include "core/include/Forward.h"
#include "core/include/EventPublisher.h"
#include "vision/include/Common.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT Detector : public core::EventPublisher
{
public:
    /** Run the detector on the input image, debug results to output Image
     *
     *  @param input   The image to run the detector on
     *  @param output  Debug image will be copied to this image
     */
    virtual void processImage(Image* input, Image* output = 0) = 0;

    /** Get the set of properties for this object */
    virtual core::PropertySetPtr getPropertySet();

    /** Transfrom from OpenCV image coordinates to AI cordinates 
     *
     *  AI coordinates have the origin at the center, +Y up and +X to the 
     *  right.  Y goes from 1 to -1 and X goes from width/height to
     *  -width/height.
     *
     *  @param image
     *      The image the coordinates were taken from.
     *  @param imageX
     *      The x coordinate in the image frame.
     *  @param imageY
     *      The y coordinate in the image frame.
     *  @param outX
     *      The value which will be changed the X coordinate in AI frame
     *  @param outY
     *      The value which will be changed the Y coordinate in AI frame
     */
    static void imageToAICoordinates(const Image* image, 
				     const int& imageX, const int& imageY,
				     double& outX, double& outY);
    
protected:
    Detector(core::EventHubPtr eventHub = core::EventHubPtr());

private:
    /** Holds all the properties for this detector */
    core::PropertySetPtr m_propertySet;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_DETECTOR_H_02_07_2008

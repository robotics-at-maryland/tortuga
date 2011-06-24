/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/include/Utility.h
 */

#ifndef RAM_VISION_UTILITY_H
#define RAM_VISION_UTILITY_H

#include "vision/include/Forward.h"
#include "vision/include/RegionOfInterest.h"

namespace ram {
namespace vision {


struct Utility
{

    /** Counts the number of white pixels in the sub area of the image */
    static int countWhitePixels(Image* source,
                                int upperLeftX, int upperLeftY,
                                int lowerRightX, int lowerRightY);
    
    static int countWhitePixels(Image* source, RegionOfInterest roi);

    static double distanceToObjective(double actualWidth, double pixelWidth,
                                      double fieldOfView);


    // /** Balance the white values */
    // static void balanceWhite();
    
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_UTILITY_H

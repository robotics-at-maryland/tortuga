/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/Utility.h
 */

#ifndef RAM_VISION_TEST_UTILITY_H_04_09_2008
#define RAM_VISION_TEST_UTILITY_H_04_09_2008

// Library Includes
#include "cxtypes.h"
#include "cxcore.h"

// Project Includes
#include "vision/include/Common.h"

namespace ram {
namespace vision {    
    
void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B);

void drawSquare(vision::Image* image, int x, int y, int width, int height,
                double angle, CvScalar color);

enum TestSuitType
{
    Heart,
    Spade,
    Club,
    Diamond,
    None
};
    
void drawBin(vision::Image* image, int x, int y, int width, double angle,
             TestSuitType suitType = None);

void drawCircle(vision::Image* image, int x, int y, int radius,
		CvScalar color);

/** Get the process ID of the current process */    
int getPid();
    
} // namespace vision
} // namespace ram

    
#endif // RAM_VISION_TEST_UTILITY_H_04_09_2008

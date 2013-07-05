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
//#include "cxtypes.h" McBryan 6-6-2013 says this doesn't exist I dont know what it does
#include "cxcore.h"

// Project Includes
#include "vision/include/Common.h"

namespace ram {
namespace vision {    
    
void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B);

void makeGray(vision::Image* image, unsigned char G);

void drawSquare(vision::Image* image, int x, int y, int width, int height,
                double angle, CvScalar color);

enum TestSuitType
{
    Heart,
    Spade,
    Club,
    Diamond,
    Tank,
    Ship,
    Aircraft,
    Factory,
    None
};
    
void drawBin(vision::Image* image, int x, int y, int width, double angle,
             TestSuitType suitType = None);

void drawTarget(vision::Image* image, int x, int y, int height = 100,
                int width = -1);
    
void drawCircle(vision::Image* image, int x, int y, int radius,
                CvScalar color);

void drawLine(vision::Image* image, int x1, int y1, int x2, int y2,
              int lineWidth, CvScalar color);

/** Get the process ID of the current process */    
int getPid();
    
} // namespace vision
} // namespace ram

    
#endif // RAM_VISION_TEST_UTILITY_H_04_09_2008

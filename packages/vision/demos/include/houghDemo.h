/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/demos/include/houghDemo.h
 */


#ifndef RAM_VISION_HOUGHDEMO_H_02_01_2007
#define RAM_VISION_HOUGHDEMO_H_02_01_2007

// Library Includes
#include "cv.h"
#include "highgui.h"

void hough(IplImage*);
void LU2filter(IplImage*);
void sobel(IplImage*);
void mask(IplImage*, int, unsigned int);
void find_green(IplImage*);

#endif // RAM_VISION_HOUGHDEMO_H_02_01_2007

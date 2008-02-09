/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/MockDetector.cxx
 */

// Project Includes
#include "vision/test/include/MockDetector.h"
#include "vision/include/OpenCVImage.h"


MockDetector::MockDetector() :
    processCount(0),
    inputImage(new ram::vision::OpenCVImage(10, 10))
{
    
}

MockDetector::~MockDetector()
{
    delete inputImage;
}

void MockDetector::processImage(ram::vision::Image* input,
                                ram::vision::Image* output)
{
    inputImage->copyFrom(input);
    processCount++;
}

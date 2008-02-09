/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/MockDetector.h
 */

#ifndef RAM_VISION_TEST_MOCKDETECTOR_H_02_07_2008
#define RAM_VISION_TEST_MOCKDETECTOR_H_02_07_2008

// Project Includes
#include "vision/include/Detector.h"

class MockDetector : public ram::vision::Detector
{
public:
    MockDetector();
    ~MockDetector();

    /** Stores a copy of the input image, increments processCount */
    virtual void processImage(ram::vision::Image* input,
                              ram::vision::Image* output = 0);

    int processCount;
    ram::vision::Image* inputImage;
};

#endif // RAM_VISION_TEST_MOCKDETECTOR_H_02_07_2008

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestVisionRunner.cxx
 */

// System Includes
#include <unistd.h>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vision/include/VisionRunner.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/MockCamera.h"
#include "vision/test/include/MockDetector.h"
#include "vision/test/include/UnitTestChecks.h"

#include "core/include/TimeVal.h"

using namespace ram;

void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B);

SUITE(VisionRunner) {

struct VisionRunnerFixture
{
    VisionRunnerFixture() :
        image(new vision::OpenCVImage(640, 480)),
        camera(new MockCamera(image))
    {
        makeColor(image, 0, 255, 0);
    }

    ~VisionRunnerFixture()
    {
        delete image;
        delete camera;
    }
    
    vision::Image* image;
    MockCamera* camera;
};
    
TEST_FIXTURE(VisionRunnerFixture, CreateDestroy)
{
    vision::VisionRunner runner(camera);
}

TEST_FIXTURE(VisionRunnerFixture, Update)
{
    vision::VisionRunner runner(camera);

    // "Backgrounds" the camera (no real background thread, because this is
    // just a mock object)
    camera->background(0);
    
    // Update vision runner with no detectors should do nothing
    for (int i = 0; i < 10; ++i)
        camera->update(0);
    
    // Add a mock detector and make sure its called properly
    MockDetector* detector = new MockDetector();
    vision::DetectorPtr detectorPtr(detector);
    // This addition automatically starts the detector running in the background
    runner.addDetector(detectorPtr);
    
    // Have to wait for the processing thread to be waiting on the camera
    ram::core::TimeVal::sleep(500.0 / 1000.0);

    // Release the image from the camera
    camera->update(0);

    // Wait for the capture to go ahead
    ram::core::TimeVal::sleep(33.0 / 1000.0);

    // This removal stops the background thread
    runner.removeDetector(detectorPtr);


    CHECK_EQUAL(1, detector->processCount);
    CHECK_CLOSE(*image, *detector->inputImage, 0);

    // Ensure another update does nothing
    camera->update(0);
    CHECK_EQUAL(1, detector->processCount);


    camera->unbackground(true);
}
  
} // SUITE(VisionRunner)

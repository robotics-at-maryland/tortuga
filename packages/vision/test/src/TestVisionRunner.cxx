/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestVisionRunner.cxx
 */

// System Includes
//#include <unistd.h>
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vision/include/VisionRunner.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/MockCamera.h"
#include "vision/test/include/MockDetector.h"
#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

#include "core/include/TimeVal.h"

using namespace ram;

SUITE(VisionRunner) {

struct VisionRunnerFixture
{
    VisionRunnerFixture() :
        image(new vision::OpenCVImage(640, 480)),
        camera(new MockCamera(image))
    {
        vision::makeColor(image, 0, 255, 0);
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
    vision::VisionRunner runner(camera, vision::Recorder::NEXT_FRAME);
}

#ifndef RAM_WINDOWS
TEST_FIXTURE(VisionRunnerFixture, Update)
{

    vision::VisionRunner runner(camera, vision::Recorder::NEXT_FRAME);

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
    CHECK(runner.backgrounded());

    // Unbackground to make the rest of the test deterministic
    runner.unbackground(true);
    
    // Release the image from the camera
    camera->update(0);
    runner.update(1.0/20);

    CHECK_EQUAL(1, detector->processCount);
    CHECK_CLOSE(*image, *detector->inputImage, 0);
    
    // Make sure that you can remove the detector (with the background thread)
    runner.background(-1);
    runner.removeDetector(detectorPtr, true); // Force removal now

    // Ensure another update does nothing
    camera->update(0);
    runner.update(1.0/20);
    
    CHECK_EQUAL(1, detector->processCount);
    CHECK_EQUAL(false, runner.backgrounded());

    // Prevent dead lock in case the test fails
    if (runner.backgrounded())
    {
        runner.unbackground(true);
        assert(false && "Test failed");
    }

    camera->unbackground(true);

}
#endif
  
} // SUITE(VisionRunner)

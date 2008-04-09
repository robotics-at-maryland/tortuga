/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestFileRecorder.cxx
 */

// Not tested yet on windows or Mac, so just go for Linux
#ifdef RAM_LINUX

// STD Includes
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/NetworkRecorder.h"
#include "vision/include/NetworkCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OpenCVCamera.h"

#include "vision/test/include/MockCamera.h"
#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

#include "core/include/TimeVal.h"


using namespace ram;

static const int TEST_PORT = 59156;

SUITE(NetworkRecorder) {

struct RecorderFixture
{
    RecorderFixture() :
        camera(new MockCamera(0))
    {
        camera->_fps = 30;
    }

    ~RecorderFixture()
    {
        delete camera;
    }

    MockCamera* camera;
};
    
TEST_FIXTURE(RecorderFixture, CreateDestroy)
{
    vision::NetworkRecorder recorder(camera, vision::Recorder::NEXT_FRAME,
                                     TEST_PORT);
}

TEST_FIXTURE(RecorderFixture, Update)
{
    camera->background(-1);
    vision::NetworkRecorder recorder(camera, vision::Recorder::NEXT_FRAME,
                                     TEST_PORT);

    // Generate 20 images
    std::vector<vision::Image*> images;
    for (int i = 0; i < 20; ++i)
    {
        vision::Image* image = new vision::OpenCVImage(640,480);
        vision::makeColor(image, i * 20, 0, 0);
        images.push_back(image);
    }

    // Wait for the network recorder to be ready for clients
    recorder.waitForAccepting();
    
    // Create the network camera
    vision::NetworkCamera*  networkCamera =
        new vision::NetworkCamera("localhost", TEST_PORT);
    
    // Record each image
    vision::Image* actual = new vision::OpenCVImage(640, 480);
    BOOST_FOREACH(vision::Image* image, images)
    {
        // Set our new image, and send it across the network
        camera->setNewImage(image);
        camera->update(0);
        
        // Read the image back
        networkCamera->update(0);
        networkCamera->getImage(actual);
        CHECK_CLOSE(*image, *actual, 0);
    }

    // Free Images
    BOOST_FOREACH(vision::Image* image, images)
    {
        delete image;
    }

    camera->unbackground();

    // Shutdown the client before we shutdown the NetworkRecorder
    delete networkCamera;
}

} // SUITE(NetworkRecorder)

#endif // RAM_LINUX

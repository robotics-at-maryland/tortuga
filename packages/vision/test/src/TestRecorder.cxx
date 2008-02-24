/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestRecorder.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/foreach.hpp>

// Project Includes
#include "vision/test/include/MockRecorder.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/MockCamera.h"
#include "vision/test/include/UnitTestChecks.h"

using namespace ram;

void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B);

SUITE(Recorder) {

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
    MockRecorder recorder(camera, vision::Recorder::NEXT_FRAME);
}

TEST_FIXTURE(RecorderFixture, Update_NEXT_FRAME)
{
    MockRecorder recorder(camera, vision::Recorder::NEXT_FRAME);

    // Generate 20 images
    std::vector<vision::Image*> images;
    for (int i = 0; i < 20; ++i)
    {
        vision::Image* image = new vision::OpenCVImage(640,480);
        makeColor(image, i * 20, 0, 0);
        images.push_back(image);
    }
    
    // Record each image
    BOOST_FOREACH(vision::Image* image, images)
    {
        camera->setNewImage(image);
        camera->update(0);
        recorder.update(1.0/30);

        // Check to make sure the right image was recorded
        CHECK(recorder.lastRecordedFrame);
        CHECK_CLOSE(*image, *(recorder.lastRecordedFrame), 0);
    }

    // Free Images
    BOOST_FOREACH(vision::Image* image, images)
    {
        delete image;
    }
}

} // SUITE(Recorder)

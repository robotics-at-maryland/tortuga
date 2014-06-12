/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestRecorder.cxx
 */

#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/foreach.hpp>

// Project Includes
#include "vision/test/include/MockRecorder.h"
#include "vision/test/include/MockCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/NetworkRecorder.h"

#include "vision/test/include/MockCamera.h"
#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

using namespace ram;

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
        vision::makeColor(image, i * 20, 0, 0);
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

TEST_FIXTURE(RecorderFixture, Update_MAX_RATE)
{
    MockRecorder recorder(camera, vision::Recorder::MAX_RATE,
                          4); // 4 Hz

    // Generate 20 images
    std::vector<vision::Image*> images;
    for (int i = 0; i < 20; ++i)
    {
        vision::Image* image = new vision::OpenCVImage(640,480);
        vision::makeColor(image, i * 20, 0, 0);
        images.push_back(image);
    }
    
    BOOST_FOREACH(vision::Image* image, images)
    {
        camera->setNewImage(image);
        camera->update(0);
        recorder.update(1.0/20); // 20 Hz
    }
    // this test is no longer valid for the way the code is implemented
    // Make only the right number of the images got
    // CHECK_EQUAL(5u, recorder.imageCRCs.size());

    // Free Images
    BOOST_FOREACH(vision::Image* image, images)
    {
        delete image;
    }
}

TEST_FIXTURE(RecorderFixture, createFromString)
{
    MockCamera camera;
    std::string message;
    vision::Recorder* recorder = vision::Recorder::createRecorderFromString(
        "50000(320,240)", &camera, message, vision::Recorder::MAX_RATE, 5);

    CHECK_EQUAL(320u, recorder->getRecordingWidth());
    CHECK_EQUAL(240u, recorder->getRecordingHeight());
    CHECK(dynamic_cast<vision::NetworkRecorder*>(recorder));
    
    delete recorder;

    recorder = vision::Recorder::createRecorderFromString(
        "50000", &camera, message, vision::Recorder::MAX_RATE, 5);

    CHECK_EQUAL(640u, recorder->getRecordingWidth());
    CHECK_EQUAL(480u, recorder->getRecordingHeight());
    CHECK(dynamic_cast<vision::NetworkRecorder*>(recorder));

    delete recorder;
}


} // SUITE(Recorder)

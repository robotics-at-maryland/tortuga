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
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/FFMPEGRecorder.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OpenCVCamera.h"

#include "vision/test/include/MockCamera.h"
#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"


using namespace ram;
namespace bf = boost::filesystem;

SUITE(FFMPEGRecorder) {

static int IMAGE_COUNT = 10;
    
struct RecorderFixture
{
    RecorderFixture() :
        camera(new MockCamera(0)),
        filename("")
    {
        std::stringstream ss;
        ss << "FFMPEGRecorderTestMovie" << "_" << vision::getPid() << ".avi";
        filename = ss.str();
        camera->_fps = 30;
    }

    ~RecorderFixture()
    {
        // Remove movie file
        bf::path movieFile(filename);
        if (bf::exists(movieFile))
            bf::remove(movieFile);
            
        delete camera;
    }

    MockCamera* camera;
    std::string filename;
};
    
TEST_FIXTURE(RecorderFixture, CreateDestroy)
{
    vision::FFMPEGRecorder recorder(camera, vision::Recorder::NEXT_FRAME,
                                    filename);
}

TEST_FIXTURE(RecorderFixture, Update)
{
    vision::FFMPEGRecorder* recorder = new
        vision::FFMPEGRecorder(camera, vision::Recorder::NEXT_FRAME, filename);
    recorder->unbackground(true);

    // Generate IMAGE_COUNT images
    std::vector<vision::Image*> images;
    for (int i = 0; i < IMAGE_COUNT; ++i)
    {
        vision::Image* image = new vision::OpenCVImage(640,480);
        vision::makeColor(image, i * 20, i * 10, i * 5);
        images.push_back(image);
    }

    // Record each image
    BOOST_FOREACH(vision::Image* image, images)
    {
        camera->setNewImage(image);
        camera->update(0);
        recorder->update(1.0/30);
    }

    // Check Results
    vision::Image* actual = new vision::OpenCVImage(640, 480);
    vision::OpenCVCamera movieCamera(filename.c_str());

    CHECK_EQUAL(30u, movieCamera.fps());
    CHECK_EQUAL(640u, movieCamera.width());
    CHECK_EQUAL(480u, movieCamera.height());

    BOOST_FOREACH(vision::Image* expectedImage, images)
    {
        movieCamera.update(0);
        movieCamera.getImage(actual);
        CHECK_CLOSE(*expectedImage, *actual, 4);
    }
    delete actual;

    // Free Images
    BOOST_FOREACH(vision::Image* image, images)
    {
        delete image;
    }
    
    delete recorder;
}

} // SUITE(FFMPEGRecorder)

#endif // RAM_LINUX

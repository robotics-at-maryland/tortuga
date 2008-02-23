/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestRecorder.cxx
 */

// STD Includes
#include <sstream>
#include <string>
#include <cstdio>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/Recorder.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OpenCVCamera.h"

#include "vision/test/include/MockCamera.h"
#include "vision/test/include/UnitTestChecks.h"

#include "core/include/TimeVal.h"

// System Includes
#ifdef RAM_POSIX
#include <unistd.h>
#else
#include <windows.h>
#endif

using namespace ram;
namespace bf = boost::filesystem;


void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B);

int getPid()
{
#ifdef RAM_POSIX
    return getpid();
#else
    return (int)GetCurrentThreadId();
#endif
}


SUITE(Recorder) {

struct RecorderFixture
{
    RecorderFixture() :
        camera(new MockCamera(0)),
        filename("")
    {
        std::stringstream ss;
        ss << "RecorderTestMovie" << "_" << getPid() << ".avi";
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
    vision::Recorder recorder(camera, vision::Recorder::NEXT_FRAME,
                              filename);
}

TEST_FIXTURE(RecorderFixture, Update)
{
    vision::Recorder recorder(camera, vision::Recorder::NEXT_FRAME,
                              filename);
    recorder.unbackground(true);

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
        CHECK_CLOSE(*expectedImage, *actual, 1);
    }    
    delete actual;

    // Free Images
    BOOST_FOREACH(vision::Image* image, images)
    {
        delete image;
    }
    
}

} // SUITE(Recorder)

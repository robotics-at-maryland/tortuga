/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestFileRecorder.cxx
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
#include "vision/include/RawFileRecorder.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/RawFileCamera.h"

#include "vision/test/include/MockCamera.h"
#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

//#include "core/include/TimeVal.h"

using namespace ram;
namespace bf = boost::filesystem;

SUITE(RawFileRecorder) {

static int IMAGE_COUNT = 10;
    
struct Fixture
{
    Fixture() :
        camera(new MockCamera(0)),
        filename("")
    {
        std::stringstream ss;
        ss << "RawFileCameraTestMovie" << "_" << vision::getPid() << ".rmv";
        filename = ss.str();
        camera->_fps = 30;
    }

    ~Fixture()
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
    

TEST_FIXTURE(Fixture, Update)
{
    vision::RawFileRecorder recorder(camera, vision::Recorder::NEXT_FRAME,
                                  filename);
    recorder.unbackground(true);

    // Generate IMAGE_COUNT images
    std::vector<vision::Image*> images;
    for (int i = 0; i < IMAGE_COUNT; ++i)
    {
        vision::Image* image = new vision::OpenCVImage(640,480,
                                                       vision::Image::PF_BGR_8);
        vision::makeColor(image, i * 20, 0, 0);
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
    vision::Image* actual = new vision::OpenCVImage(640, 480,
                                                    vision::Image::PF_BGR_8);
    vision::RawFileCamera movieCamera(filename.c_str());

    // Lets check some other stuff
    CHECK_CLOSE(30.0, movieCamera.fps(), 0.01);
    //CHECK_CLOSE(IMAGE_COUNT/30.0, movieCamera.duration(), 0.01);
    
    CHECK_EQUAL(30u, movieCamera.fps());
    CHECK_EQUAL(640u, movieCamera.width());
    CHECK_EQUAL(480u, movieCamera.height());

    int frameNum = 0;
    BOOST_FOREACH(vision::Image* expectedImage, images)
    {
        movieCamera.update(0);
        movieCamera.getImage(actual);

        // Make sure we got the right frame
        CHECK_CLOSE(*expectedImage, *actual, 1.5);

        // Make sure we got the right time
        CHECK_CLOSE(frameNum/30.0, movieCamera.currentTime(), 0.01);

        frameNum++;
    }    
    delete actual;

    // Free Images
    BOOST_FOREACH(vision::Image* image, images)
    {
        delete image;
    }
    
}

} // SUITE(RawFileRecorder)

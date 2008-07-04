/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestDuctDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

// Project Includes
#include "vision/include/DuctDetector.h"
//#include "vision/include/OpenCVImage.h"
#include "vision/include/Image.h"
//#include "vision/include/Events.h"
//#include "vision/test/include/Utility.h"

//#include "core/include/EventHub.h"

using namespace ram;

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images" / "testduct";
}

struct DuctDetectorFixture
{
    DuctDetectorFixture() :
//        found(false),
//        almostHit(false),
//        event(vision::DuctEventPtr()),
//        input(640, 480),
//        eventHub(new core::EventHub()),
        detector()
    {/*
        eventHub->subscribeToType(vision::EventType::LIGHT_FOUND,
            boost::bind(&DuctDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::LIGHT_LOST,
            boost::bind(&DuctDetectorFixture::lostHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::LIGHT_ALMOST_HIT,
            boost::bind(&DuctDetectorFixture::almostHitHandler, this, _1));*/
    }

  /*  void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::DuctEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::DuctEventPtr();
    }

    void almostHitHandler(core::EventPtr event)
    {
        almostHit = true;
    }
    
    bool found;
    bool almostHit;
    vision::DuctEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub; */
    vision::DuctDetector detector;
};

SUITE(DuctDetector) {
    
TEST_FIXTURE(DuctDetectorFixture, getAlignment)
{
	vision::Image* input =
	 vision::Image::loadFromFile(
	    (getImagesDir() / "alignment.png").string());
    // Blue Image with red circle in the center
	detector.processImage(input);

    CHECK(detector.getAligned());
    //std::cout << "Rotation 1: " << detector.getRotation() << "\n";
	delete input;
	
	
	vision::Image* input2 = 
	vision::Image::loadFromFile(
	    (getImagesDir() / "unal0.png").string());
    // Blue Image with red circle in the center
	detector.processImage(input2);

	//std::cout << "Rotation 2: " << detector.getRotation() << "\n";
    CHECK(!detector.getAligned());
	delete input2;
	
	
	vision::Image* input3 = 
	vision::Image::loadFromFile(
	    (getImagesDir() / "unal1.png").string());
    // Blue Image with red circle in the center
	detector.processImage(input3);
	
	//std::cout << "Rotation 3: " << detector.getRotation() << "\n";
    CHECK(!detector.getAligned());
	delete input3;
	
	
	
	vision::Image* input4 = 
	vision::Image::loadFromFile(
	    (getImagesDir() / "unal2.png").string());
    // Blue Image with red circle in the center
	detector.processImage(input4);
	
	//std::cout << "Rotation 4: " << detector.getRotation() << "\n";
        CHECK(!detector.getAligned());
        
	delete input4;
}

/*
TEST_FIXTURE(DuctDetectorFixture, Events_LIGHT_ALMOST_HIT)
{
    // Blue Image with red circle in the center
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/2, 240, 140);

    // Process it
    detector.processImage(&input);

    double expectedX = 0 * 640.0/480.0;
    double expectedY = 0;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    // Check the events

    // LIGHT_ALMOST_HIT
    CHECK(almostHit);
    
    // LIGHT_FOUND
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(1.058, event->range, 0.1);
    CHECK_CLOSE(math::Degree(0), event->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(0), event->elevation, math::Degree(0.4));
}
*/
} // SUITE(DuctDetector)

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
#include "vision/include/DownwardDuctDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Image.h"
#include "vision/include/Events.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"

using namespace ram;

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images" / "testduct";
}

/** Draws the air duct from the side */
enum DuctDirection {
    DUCT_VERTICAL,
    DUCT_HORIZONTAL
};

void drawSideDuct(vision::Image* image, int x, int y, int width = 100,
                  DuctDirection mode = DUCT_VERTICAL)
{
    // Black square
    drawSquare(image, x, y, width, width, 0, CV_RGB(0,0,0));

    double stripeWidth = ((double)width/10.0);
    double stripeOffset = (double)width/2 - stripeWidth/2;

    if (DUCT_HORIZONTAL == mode)
    {
        // Left Yellow Stripe
        drawSquare(image, (int)(x - stripeOffset), y, (int)stripeWidth, width,
                   0, CV_RGB(255, 255, 0));
        
        // Right Yellow Stripe
        drawSquare(image, (int)(x + stripeOffset), y, (int)stripeWidth, width,
                   0, CV_RGB(255, 255, 0));
    }
    else // DUCT_VERTICAL
    {
        // Left Yellow Stripe
        drawSquare(image, x, (int)(y - stripeOffset), width, (int)stripeWidth,
                   0, CV_RGB(255, 255, 0));
        
        // Right Yellow Stripe
        drawSquare(image, x, (int)(y + stripeOffset), width, (int)stripeWidth,
                   0, CV_RGB(255, 255, 0));
    }
}

void drawFrontDuct(vision::Image* image, int x, int y, int width = 100)
{
    // Draw outer ring
    drawSideDuct(image, x, y, width, DUCT_VERTICAL);
    drawSideDuct(image, x, y, width,  DUCT_HORIZONTAL);

    // Draw inner ring
    double innerWidth = (double)width * 0.5;
    drawSideDuct(image, x, y, (int)innerWidth, DUCT_VERTICAL);
    drawSideDuct(image, x, y, (int)innerWidth, DUCT_HORIZONTAL);
}

void drawDuctBottom(vision::Image* image, int minX, int maxX, int minY, int maxY, int width = 20)
{
    // Bottom Yellow Stripe
    drawSquare(image, (minX + maxX)/2, minY + width/2, maxX - minX, width,
                0, CV_RGB(255, 255, 0));

    drawSquare(image, (minX + maxX)/2, maxY - width/2, maxX - minX, width,
                0, CV_RGB(255, 255, 0));

    drawSquare(image, (minX + maxX)/2, (minY + maxY) / 2, width, maxY - minY,
                0, CV_RGB(255, 255, 0));
}


struct DuctDetectorFixture
{
    DuctDetectorFixture() :
       found(false),
        event(vision::DuctEventPtr()),
        //input(640, 480),
        eventHub(new core::EventHub()),
        detector(eventHub),
        downDetector(eventHub)
    {
        eventHub->subscribeToType(vision::EventType::DUCT_FOUND,
            boost::bind(&DuctDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::DUCT_LOST,
            boost::bind(&DuctDetectorFixture::lostHandler, this, _1));

        eventHub->subscribeToType(vision::EventType::DOWN_DUCT_FOUND,
            boost::bind(&DuctDetectorFixture::downFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::DOWN_DUCT_LOST,
            boost::bind(&DuctDetectorFixture::downLostHandler, this, _1));

    }

    void downFoundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::DuctEvent>(event_);    
//        printf("down found\n");
    }

    void downLostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::DuctEventPtr();
//        printf("down lost\n");
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::DuctEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::DuctEventPtr();
    }
    
    bool found;
    vision::DuctEventPtr event;
    //vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::DuctDetector detector;
    vision::DownwardDuctDetector downDetector;
};

SUITE(DuctDetector) {

TEST_FIXTURE(DuctDetectorFixture, downwardDuctDetector)
{
    vision::OpenCVImage input(640,480);
    vision::OpenCVImage output(640,480);
    
    vision::makeColor(&input, 0, 0, 255);
    downDetector.processImage(&input, &output);
    CHECK(!event);
    CHECK(!found);
    
    drawDuctBottom(&input, 0, 640, 0, 480);
    downDetector.processImage(&input, &output);
    CHECK(event);
    CHECK(found);
    if (event)
    {
        CHECK_CLOSE(0, event->x, 0.2);
        CHECK_CLOSE(0, event->y, 0.2);
    }
    
//    vision::Image::showImage(&output);
    vision::makeColor(&input, 0, 0, 255);
    downDetector.processImage(&input, &output);
    CHECK(!event);
    CHECK(!found);
//    printf("Finished downwardDuctDetector\n");
}

TEST_FIXTURE(DuctDetectorFixture, downTest2)
{
    vision::OpenCVImage input(640,480);
    vision::OpenCVImage output(640,480);
    
    vision::makeColor(&input, 0, 0, 255);
    
    drawDuctBottom(&input, 0, 320, 0, 240);
    downDetector.processImage(&input, &output);
    CHECK(event);
    CHECK(found);
    if (event)
    {
        CHECK_CLOSE(-.66, event->x, 0.2);
        CHECK_CLOSE(.5, event->y, 0.2);
    }
    
//    vision::Image::showImage(&output);
    vision::makeColor(&input, 0, 0, 255);
    downDetector.processImage(&input, &output);
    CHECK(!event);
    CHECK(!found);
//    printf("Finished downTest2\n");
}

TEST_FIXTURE(DuctDetectorFixture, getAlignment)
{
    vision::OpenCVImage output(480, 640);

    vision::Image* input =
     vision::Image::loadFromFile(
        (getImagesDir() / "alignment.png").string());
    detector.processImage(input, &output);
    
    //vision::Image::showImage(&output);
    
    //CHECK(detector.getAligned());
    //CHECK_CLOSE(0, detector.getX(), 0.2);
    //CHECK_CLOSE(0, detector.getY(), 0.2);
    //CHECK_CLOSE(0, detector.getRotation(), 0.1);
    delete input;
    
    
    
    vision::Image* input2 = 
    vision::Image::loadFromFile(
        (getImagesDir() / "unal0.png").string());
    // Blue Image with red circle in the center
    detector.processImage(input2, &output);
    
    //vision::Image::showImage(&output);

    CHECK_CLOSE(0, detector.getX(), 0.2);
    CHECK_CLOSE(0, detector.getY(), 0.2);
    CHECK(!detector.getAligned());
    delete input2;
    
    
    
    vision::Image* input3 = 
    vision::Image::loadFromFile(
        (getImagesDir() / "unal1.png").string());
    // Blue Image with red circle in the center
    detector.processImage(input3, &output);
    
    //vision::Image::showImage(&output);
    
    CHECK_CLOSE(0, detector.getX(), 0.2);
    CHECK_CLOSE(0, detector.getY(), 0.2);
    CHECK_CLOSE(1, detector.getRotation(), 1);
    CHECK(!detector.getAligned());
    delete input3;
    
    
    
    vision::Image* input4 = 
    vision::Image::loadFromFile(
        (getImagesDir() / "unal2.png").string());
    // Blue Image with red circle in the center
    detector.processImage(input4, &output);
    
    //vision::Image::showImage(&output);
    
    CHECK_CLOSE(0, detector.getX(), 0.2);
    CHECK_CLOSE(0, detector.getY(), 0.2);
    CHECK_CLOSE(1, detector.getRotation(), 1);
    CHECK(!detector.getAligned());
    delete input4;
    
    
    
        
    vision::Image* input5 = 
    vision::Image::loadFromFile(
        (getImagesDir() / "unal3.png").string());
    // Blue Image with red circle in the center
    detector.processImage(input5, &output);
    
    //vision::Image::showImage(&output);
    
    CHECK_CLOSE(0, detector.getX(), 0.2);
    CHECK_CLOSE(0, detector.getY(), 0.2);
    CHECK_CLOSE(-1, detector.getRotation(), 1);
    //CHECK_CLOSE(0.49, detector.getRange(), 0.05);
    CHECK(!detector.getAligned());
    delete input5;
    
    
    /*
    
    vision::Image* input6 = 
    vision::Image::loadFromFile(
        (getImagesDir() / "negativeAlign.png").string());
    // Blue Image with red circle in the center
    detector.processImage(input6, &output);
    
    //vision::Image::showImage(&output);
    
    CHECK_CLOSE(-1, detector.getRotation(), 1);
    //CHECK_CLOSE(0.49, detector.getRange(), 0.05);
    CHECK(!detector.getAligned());
    delete input6;
    
    */
    
    
    vision::Image* input7 = 
    vision::Image::loadFromFile(
        (getImagesDir() / "unal4.png").string());
    // Blue Image with red circle in the center
    detector.processImage(input7, &output);
    
    //vision::Image::showImage(&output);
    
    CHECK_CLOSE(0, detector.getX(), 0.2);
    CHECK_CLOSE(0, detector.getY(), 0.2);
    CHECK_CLOSE(-1, detector.getRotation(), 1);
    //CHECK_CLOSE(0.49, detector.getRange(), 0.05);
    CHECK(!detector.getAligned());
    delete input7;
    
}

/*
TEST_FIXTURE(DuctDetectorFixture, range)
{
    vision::Image* input =
     vision::Image::loadFromFile(
        (getImagesDir() / "alignment.png").string());
    detector.processImage(input);

    CHECK(detector.getAligned());
    //CHECK_CLOSE(expectedValue, actualValue, maxDifference);
    CHECK_CLOSE(0, detector.getX(), 0.2);
    CHECK_CLOSE(0, detector.getY(), 0.2);
    CHECK_CLOSE(0, detector.getRotation(), 0.4);
    CHECK_CLOSE(0.32, detector.getRange(), 0.05);
    delete input;
    
    
    vision::Image* input2 = 
    vision::Image::loadFromFile(
        (getImagesDir() / "far.png").string());
    // Blue Image with red circle in the center
    detector.processImage(input2);

    CHECK_CLOSE(0, detector.getX(), 0.2);
    CHECK_CLOSE(0, detector.getY(), 0.2);
    CHECK_CLOSE(10, detector.getRotation(), 10);
    CHECK_CLOSE(0.76, detector.getRange(), 0.05);
    CHECK(detector.getAligned());
    delete input2;
}

TEST_FIXTURE(DuctDetectorFixture, Events_DUCT_FOUND)
{
    vision::Image* input =
     vision::Image::loadFromFile(
        (getImagesDir() / "alignment.png").string());
        
    // Process it
    detector.processImage(input);

    CHECK(detector.getAligned());

    // LIGHT_FOUND
    CHECK(found);
    CHECK(event);
    CHECK(event->aligned);
}

TEST_FIXTURE(DuctDetectorFixture, Events_DUCT_LOST)
{
    // No duct
    vision::OpenCVImage blank(640, 480);
    vision::makeColor(&blank, 0, 0, 255);
    
    detector.processImage(&blank);
    CHECK(!event);

    // Found duct
    vision::Image* input =
        vision::Image::loadFromFile(
            (getImagesDir() / "alignment.png").string());
        
    // Process it
    detector.processImage(input);
    CHECK(found);
    CHECK(event);
    CHECK(event->visible);
    CHECK_CLOSE(0, event->x, 0.2);
    CHECK_CLOSE(0, event->y, 0.2);
    CHECK_CLOSE(0, event->alignment, 0.4);
    
    // Lost Duct
    detector.processImage(&blank);
    CHECK(!event);
    
    // Make sure we don't get repeat Lost
    detector.processImage(input);
    CHECK(found);
    CHECK(event);
    CHECK(event->visible);
    CHECK_CLOSE(0, event->x, 0.2);
    CHECK_CLOSE(0, event->y, 0.2);
    CHECK_CLOSE(0, event->alignment, 0.4);
    
    // Now make sure we get found again
    detector.processImage(&blank);
    CHECK(!event);
}
*/
// Test some corner cases for direction

/*
TEST_FIXTURE(DuctDetectorFixture, UpperLeft)
{
    // MAKE DETECTOR PASS ME
    vision::OpenCVImage image(640, 480);
    vision::OpenCVImage output(480, 640);
    makeColor(&image, 0, 0, 255);
    drawSideDuct(&image, 640/4, 480/4);

    detector.processImage(&image, &output);
    double expectedX = -0.5 * 640.0/480.0;;
    double expectedY = 0.5;
    
    //CHECK(!detector.getAligned());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
}

TEST_FIXTURE(DuctDetectorFixture, LowerRight)
{
    // MAKE DETECTOR PASS ME
    vision::OpenCVImage image(640, 480);
    vision::OpenCVImage output(480, 640);
    makeColor(&image, 0, 0, 255);
    drawSideDuct(&image, 640 - 640/4, 480/4 * 3);

    detector.processImage(&image, &output);
    double expectedX = 0.5 * 640.0/480.0;;
    double expectedY = -0.5;
    
    //CHECK(!detector.getAligned());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
}

TEST_FIXTURE(DuctDetectorFixture, UpperLeftFront)
{
    // MAKE DETECTOR PASS ME
    vision::OpenCVImage image(640, 480);
    vision::OpenCVImage output(480, 640);
    makeColor(&image, 0, 0, 255);
    drawFrontDuct(&image, 640/4, 480/4);

    detector.processImage(&image, &output);
    double expectedX = -0.5 * 640.0/480.0;
    double expectedY = 0.5;
    
    //CHECK(detector.getAligned());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
}

TEST_FIXTURE(DuctDetectorFixture, LowerRightFront)
{
    // MAKE DETECTOR PASS ME
    vision::OpenCVImage image(640, 480);
    vision::OpenCVImage output(480, 640);
    makeColor(&image, 0, 0, 255);
    drawFrontDuct(&image, 640 - 640/4, 480/4 * 3);

    detector.processImage(&image, &output);
    double expectedX = 0.5 * 640.0/480.0;
    double expectedY = -0.5;
    
    //CHECK(detector.getAligned());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
}*/


} // SUITE(DuctDetector)

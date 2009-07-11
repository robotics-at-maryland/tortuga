/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/BasicWW2Detector.cpp
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/FANNWW2Detector.h"
#include "vision/test/include/Utility.h"

using namespace ram;
/*
static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "references";
}
*/
SUITE(FANNWW2Detector) {

struct FANNSymbolDetectorFixture
{
    FANNSymbolDetectorFixture() :
        detector(core::ConfigNode::fromString("{'training' : 1}"))
    {}

    vision::FANNWW2Detector detector;
};

    
TEST_FIXTURE(FANNSymbolDetectorFixture, needSquareCropped)
{
    CHECK_EQUAL(false, detector.needSquareCropped());
}

TEST_FIXTURE(FANNSymbolDetectorFixture, getNumberFeatures)
{
    CHECK_EQUAL(4, detector.getNumberFeatures());
}

TEST_FIXTURE(FANNSymbolDetectorFixture, getOutputCount)
{
    CHECK_EQUAL(4, detector.getOutputCount());
}

TEST_FIXTURE(FANNSymbolDetectorFixture, getImageFeatures)
{
    // Test Wide images, with square in three corners and on on the edge
    vision::OpenCVImage src(192, 96);
    vision::makeColor(&src, 0, 0, 0);
    vision::drawSquare(&src, 12, 12, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src, 12, 83, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src, 180, 12, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src, 192/2, 12, 24, 24, 0.0, CV_RGB(255,255,255));

    float expectedFeatures[] = {2.0, 3.0, 0.75, 0.25};
    float results[] = {0, 0, 0, 0};

    CHECK_EQUAL(192u, src.getWidth());
    CHECK_EQUAL(96u, src.getHeight());
    detector.getImageFeatures(&src, results);
    CHECK_ARRAY_CLOSE(expectedFeatures, results, 4, 0.1);

    
    // Test tall images, with square in three corners on the edge
    vision::OpenCVImage src2(96, 192);
    vision::makeColor(&src2, 0, 0, 0);
    vision::drawSquare(&src2, 12, 12, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src2, 84, 12, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src2, 12, 180, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src2, 12, 192/2, 24, 24, 0.0, CV_RGB(255,255,255));

    float expectedFeatures2[] = {2.0, 3.1, 0.75, 0.255};
    float results2[] = {0, 0, 0, 0};

    CHECK_EQUAL(96u, src2.getWidth());
    CHECK_EQUAL(192u, src2.getHeight());
    detector.getImageFeatures(&src2, results2);
    CHECK_ARRAY_CLOSE(expectedFeatures2, results2, 4, 0.1);
}
/*
 Can't Seem to train a network for this
TEST(Detection)
{
    // Actually test to see if it can recognize things
    vision::FANNWW2Detector detector(
        core::ConfigNode::fromString("{'nueralNetworkFile' : 'test.irn'}"));

    // Load refernece images
    vision::Image* ship = vision::Image::loadFromFile(
        (getImagesDir() / "ship_fullsize_bw.png").string());
    vision::Image* aircraft = vision::Image::loadFromFile(
        (getImagesDir() / "aircraft_fullsize_bw.png").string());
    vision::Image* tank = vision::Image::loadFromFile(
        (getImagesDir() / "tank_fullsize_bw.png").string());
    vision::Image* factory = vision::Image::loadFromFile(
        (getImagesDir() / "factory_fullsize_bw.png").string());

    // Run them through the detector and check the results
    std::cout << "TEST RUN SHIP" << std::endl;
    detector.processImage(ship);
    CHECK_EQUAL(vision::Symbol::SHIP, detector.getSymbol());
    std::cout << "TEST RUN AIRCRAFT" << std::endl;
    detector.processImage(aircraft);
    CHECK_EQUAL(vision::Symbol::AIRCRAFT, detector.getSymbol());
    std::cout << "TEST RUN TANK" << std::endl;
    detector.processImage(tank);
    CHECK_EQUAL(vision::Symbol::TANK, detector.getSymbol());
    std::cout << "TEST RUN FACTORY" << std::endl;
    detector.processImage(factory);
    CHECK_EQUAL(vision::Symbol::FACTORY, detector.getSymbol());
    std::cout << "TEST RUN DONE" << std::endl;
}
*/
} // SUITE(FANNWW2Detector)

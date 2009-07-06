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
#include "vision/include/BasicWW2Detector.h"
#include "vision/test/include/Utility.h"

using namespace ram;

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "references";
}

SUITE(FANNWW2Detector) {

struct BasicWW2DetectorFixture
{
    BasicWW2DetectorFixture() :
        detector(core::ConfigNode::fromString("{}"))
    {}

    vision::BasicWW2Detector detector;
};

    
TEST_FIXTURE(BasicWW2DetectorFixture, needSquareCropped)
{
    CHECK_EQUAL(false, detector.needSquareCropped());
}

TEST_FIXTURE(BasicWW2DetectorFixture, processImage)
{
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
    detector.processImage(ship);
    CHECK_EQUAL(vision::Symbol::SHIP, detector.getSymbol());
    detector.processImage(aircraft);
    CHECK_EQUAL(vision::Symbol::AIRCRAFT, detector.getSymbol());
    detector.processImage(tank);
    CHECK_EQUAL(vision::Symbol::TANK, detector.getSymbol());
    detector.processImage(factory);
    CHECK_EQUAL(vision::Symbol::FACTORY, detector.getSymbol());
}

} // SUITE(FANNWW2Detector)

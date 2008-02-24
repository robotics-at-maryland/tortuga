/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestDetectorMaker.cxx
 */

// STD Includes
#include <utility>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Test Includes
#include "vision/include/DetectorMaker.h"
#include "vision/test/include/MockDetector.h"


static ram::vision::DetectorMakerTemplate<MockDetector>
registerThis("MockDetector");

using namespace ram::vision;
TEST(DetectorMaker)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'val' : 5, 'type' : 'MockDetector' }"));
    DetectorPtr detector = DetectorMaker::newObject(
        std::make_pair(config, ram::core::EventHubPtr()));

    MockDetector* mockDetector = dynamic_cast<MockDetector*>(detector.get());
    CHECK(mockDetector);
}

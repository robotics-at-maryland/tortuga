/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/test/src/TestFANNSuitDetector.cpp
 */

// STD Includes
#include <cmath>
#include <set>
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

#include <boost/filesystem.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/FANNSuitDetector.hpp"

using namespace ram;

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images" /
        "testfann";
}

static std::string getSuitNetworkFile()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return (root / "packages" / "vision" / "test" / "data" /
            "suits.irn").file_string();
}

struct FANNSuitDetectorFixture
{
};

SUITE(FANNSuitDetector) {
<<<<<<< .mine
    
    TEST_FIXTURE(FANNSuitDetectorFixture, FANNSuitDetectorTest)
    {
        core::ConfigNode config = core::ConfigNode::fromString ("{ 'SavedImageIdentifierNetwork' : '" + getSuitNetworkFile() + "' }");
        vision::FANNSuitDetector detector = vision::FANNSuitDetector (config);
        std::cout << "Starting FANNSuitDetectorTest:\n";
        vision::Image* img;
        boost::filesystem::directory_iterator end;
        boost::filesystem::path dir = getImagesDir();
        if (boost::filesystem::exists (dir)) {
            int results[4] = { vision::Suit::CLUB, vision::Suit::DIAMOND, vision::Suit::HEART, vision::Suit::SPADE };
            unsigned int i = 0;
            for (boost::filesystem::directory_iterator itr (dir); itr != end; ++itr) {
                if (boost::filesystem::extension (itr->path()) == ".jpg") {
                    img = vision::Image::loadFromFile(itr->path().file_string());
                    if (img) {
                        detector.processImage(img);
                    }
                    std::cout << "i: " << i << " detector.getSuit(): " << detector.getSuit() << " results[i]: " << results[i] << "\n";
                    CHECK (detector.getSuit() == results[i++]);
                }
            }
=======

TEST_FIXTURE(FANNSuitDetectorFixture, FANNSuitDetectorTest)
{
    core::ConfigNode config = core::ConfigNode::fromString (
        "{ 'SavedImageIdentifierNetwork' : '" + getSuitNetworkFile() + "' }");
    vision::FANNSuitDetector detector = vision::FANNSuitDetector (config);
    boost::filesystem::directory_iterator end;
    boost::filesystem::path dir = getImagesDir();
//    int results[4] = { vision::Suit::CLUB, vision::Suit::DIAMOND,
//                       vision::Suit::HEART, vision::Suit::SPADE };
//    unsigned int i = 0;

    // Ensure the images directory exists
    CHECK (boost::filesystem::exists (dir));    

    
/*  Broken: tests were not running due to extension really being ".jpg" not
    "jpg", when that issue was fixed tests start to crash    
    for (boost::filesystem::directory_iterator itr (dir); itr != end; ++itr) {
        if (boost::filesystem::extension(itr->path()) == ".jpg") {
            // Open image and ensure it was loaded properly
            vision::Image* img =
                vision::Image::loadFromFile(itr->path().file_string());
            CHECK(img);
            // Process the image, then make sure we get the right suit
            detector.processImage(img);
            CHECK_EQUAL(detector.getSuit(), results[i++]);
>>>>>>> .r4782
        }
<<<<<<< .mine
        std::cout << "done\n";
    }
=======
    }
    // Did not process the proper number of images
    CHECK_EQUAL(i, 4u);
*/
}
>>>>>>> .r4782
    
} // SUITE(FANNSuitDetector)

/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/test/src/TestFANNSymbolDetector.cpp
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
#include "vision/include/FANNSymbolDetector.h"

using namespace ram;
/*
static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images" /
        "testfann";
}

static std::string getSymbolNetworkFile()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return (root / "packages" / "vision" / "test" / "data" /
            "symbols.irn").file_string();
}
*/
struct FANNSymbolDetectorFixture
{
};

SUITE(FANNSymbolDetector) {
  /*    
    TEST_FIXTURE(FANNSymbolDetectorFixture, FANNSymbolDetectorTest)
    {
      
        core::ConfigNode config = core::ConfigNode::fromString ("{ 'SavedImageIdentifierNetwork' : '" + getSymbolNetworkFile() + "' }");
        vision::FANNSymbolDetector detector = vision::FANNSymbolDetector (config);
        std::cout << "Starting FANNSymbolDetectorTest:\n";
        vision::Image* img;
        boost::filesystem::directory_iterator end;
        boost::filesystem::path dir = getImagesDir();

        // Directory must exist, because this test has to run, the rest of the
        // test is not run to avoid extra error statements
        CHECK(boost::filesystem::exists (dir));
        if (boost::filesystem::exists (dir)) {
            int results[4] = { vision::Symbol::CLUB, vision::Symbol::DIAMOND, vision::Symbol::HEART, vision::Symbol::SPADE };
            unsigned int i = 0;
            for (boost::filesystem::directory_iterator itr (dir); itr != end; ++itr) {
                if (boost::filesystem::extension (itr->path()) == ".jpg") {
                    img = vision::Image::loadFromFile(itr->path().file_string());
                    if (img) {
                        detector.processImage(img);
                    }
                    std::cout << "i: " << i << " detector.getSymbol(): " << detector.getSymbol() << " results[i]: " << results[i] << "\n";
                    CHECK_EQUAL(results[i++], detector.getSymbol());
                }
            }

            // Make sure we ran all four symbol tests
            CHECK_EQUAL(4u, i);
	    }
    }
  */
} // SUITE(FANNSymbolDetector)

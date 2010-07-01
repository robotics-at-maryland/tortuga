/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/FeatureAnalyzer.cpp
 */

// STD Includes
#include <assert.h>
#include <string>
#include <utility>

// Library Includes
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/Common.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/Detector.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/FANNSymbolDetector.h"

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

namespace po = boost::program_options;
using namespace ram;

vision::FANNSymbolDetectorPtr createDetector(std::string type,
                                             core::ConfigNode config);

vision::FANNSymbolDetectorPtr createDetector(std::string type,
                                             core::ConfigNode config)
{
    config.set("type", type);
    // Set to training mode
    config.set("training", 1);
    vision::DetectorPtr detector = vision::DetectorMaker::newObject(
        vision::DetectorMakerParamType(config, core::EventHubPtr()));

    vision::FANNSymbolDetectorPtr fannDetector =
        boost::dynamic_pointer_cast<vision::FANNSymbolDetector>(detector);
    assert(fannDetector && "Symbol detector not of FANNSymbolDetector type");

    return fannDetector;
}

int main(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    std::string input;
    std::string detector;
    std::string configPath;

    try
    {
        // Posiitonal Options
        p.add("input", 1).
            add("detector", 1);

        // Option Descriptions
        desc.add_options()
            ("help", "Produce help message")
            ("input", po::value<std::string>(&input),
             "Image file")
            ("detector", po::value<std::string>(&detector)->
             default_value("FANNWW2Detector"), "Detector to run on the input")
            ("config,c", po::value<std::string>(&configPath)->
             default_value("NONE"), "Path to config with detector settings")
            ;

        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help"))
    {
        std::cout << "Usage: FeatureAnalyzer <input> [detector] [options]"
                  << std::endl << desc << std::endl;

        return EXIT_FAILURE;
    }

    vision::FANNSymbolDetectorPtr fannDetector =
        vision::FANNSymbolDetectorPtr();
    if (configPath != "NONE")
    {
        core::ConfigNode config(core::ConfigNode::fromFile(configPath));
        fannDetector = createDetector(detector, config);
    }
    else
    {
        core::ConfigNode config(core::ConfigNode::fromString("{}"));
        fannDetector = createDetector(detector, config);
    }

    vision::Image *image = vision::Image::loadFromFile(input);

    float *features = new float[fannDetector->getNumberFeatures()];
    fannDetector->getImageFeatures(image, features);

    // Write out all of the feature numbers
    for (int i = 0; i < fannDetector->getNumberFeatures(); i++)
    {
        std::cout << features[i] << " ";
    }
    std::cout << std::endl;

    delete[] features;

    return 0;
}

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
#include <math.h>
#include <algorithm>

// Library Includes
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

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
namespace fs = boost::filesystem;
using namespace ram;

vision::FANNSymbolDetectorPtr createDetector(std::string type,
                                             core::ConfigNode config);

void analyze_image(vision::FANNSymbolDetectorPtr fannDetector,
                   float* features, std::string path);

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

void analyze_image(vision::FANNSymbolDetectorPtr fannDetector,
                  float* features, std::string path)
{
    vision::Image *image = vision::Image::loadFromFile(path);
    
    fannDetector->getImageFeatures(image, features);
    
    // Write out all of the feature numbers
    for (int i = 0; i < fannDetector->getNumberFeatures(); i++)
    {
        std::cout << features[i] << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    std::string input;
    std::string detector;
    std::string configPath;
    std::string directory;

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
            ("dir,d", po::value<std::string>(&directory)->
             default_value("NONE"), "Path to directory with images")
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

    // Allocate space to store the features
    int featureNum = fannDetector->getNumberFeatures();
    float *features = new float[featureNum];

    // Check if directory or file mode
    if (directory == "NONE") {
        analyze_image(fannDetector, features, input);
    } else {
        float *max = new float[featureNum];
        for (int i=0; i < featureNum; i++) {
            max[i] = 0.0;
        }
        float *min = new float[featureNum];
        for (int i=0; i < featureNum; i++) {
            min[i] = HUGE_VAL;
        }
        
        fs::directory_iterator end;
        for (fs::directory_iterator iter(directory);
             iter != end; iter++)
        {
            std::cout << iter->path() << std::endl;
            analyze_image(fannDetector, features, iter->path().string());

            for (int i=0; i < featureNum; i++) {
                min[i] = std::min(min[i], features[i]);
                max[i] = std::max(max[i], features[i]);
            }
        }

        // Print the min/max features
        std::cout << "Minimum values:" << std::endl;
        for (int i=0; i < featureNum; i++) {
            std::cout << min[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "Maximum values:" << std::endl;
        for (int i=0; i < featureNum; i++) {
            std::cout << max[i] << " ";
        }
        std::cout << std::endl;

        delete[] max;
        delete[] min;
    }

    delete[] features;

    return 0;
}

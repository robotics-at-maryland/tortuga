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
#include <vector>

// Library Includes
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
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
#include "vision/include/Symbol.h"
#include "vision/include/SymbolDetector.h"
#include "vision/include/FANNSymbolDetector.h"

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace ram;

bool quiet = false;

vision::SymbolDetectorPtr createDetector(std::string type,
                                         core::ConfigNode config);

void analyzeImage(vision::FANNSymbolDetectorPtr fannDetector,
                  float* features, std::string path);

void processImage(vision::SymbolDetectorPtr symbolDetector,
                  std::string path);

vision::SymbolDetectorPtr createDetector(std::string type,
                                         core::ConfigNode config)
{
    config.set("type", type);
    // Set to training mode
    config.set("training", 1);
    vision::DetectorPtr detector = vision::DetectorMaker::newObject(
        vision::DetectorMakerParamType(config, core::EventHubPtr()));

    vision::SymbolDetectorPtr symbolDetector =
        boost::dynamic_pointer_cast<vision::SymbolDetector>(detector);
    assert(symbolDetector && "Detector given is not a SymbolDetector");

    return symbolDetector;
}

void processImage(vision::SymbolDetectorPtr symbolDetector, std::string path)
{
    vision::Image *image = vision::Image::loadFromFile(path);
    symbolDetector->processImage(image);

    if (!quiet)
    {
        std::string symbol(vision::Symbol::symbolToText(
                               symbolDetector->getSymbol()));
        std::cout << "Found Symbol: " << symbol << std::endl;
    }
}

void analyzeImage(vision::FANNSymbolDetectorPtr fannDetector,
                  float* features, std::string path)
{
    vision::Image *image = vision::Image::loadFromFile(path);
    
    fannDetector->getImageFeatures(image, features);
    
    // Write out all of the feature numbers
    for (int i = 0; i < fannDetector->getNumberFeatures(); i++)
    {
        if (!quiet)
            std::cout << features[i] << " ";
    }
    if (!quiet)
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
    double trim;

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
            ("q,quiet", po::bool_switch(&quiet), "Print only the end results")
            ("trim", po::value<double>(&trim)->
             default_value(5), "Percent to use for trim")
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

    trim /= 100.0;

    vision::SymbolDetectorPtr symbolDetector =
        vision::SymbolDetectorPtr();
    if (configPath != "NONE")
    {
        core::ConfigNode config(core::ConfigNode::fromFile(configPath));
        symbolDetector = createDetector(detector, config);
    }
    else
    {
        core::ConfigNode config(core::ConfigNode::fromString("{}"));
        symbolDetector = createDetector(detector, config);
    }

    // Allocate space to store the features
    vision::FANNSymbolDetectorPtr fannDetector =
        boost::dynamic_pointer_cast<vision::FANNSymbolDetector>(symbolDetector);
    int featureNum = 0;
    float *features = 0;
    if (fannDetector) {
        featureNum = fannDetector->getNumberFeatures();
        features = new float[featureNum];
    }
    // Check if directory or file mode
    if (directory == "NONE") {   
        if (fannDetector)
            analyzeImage(fannDetector, features, input);
        else
            processImage(symbolDetector, input);
    } else {
        // Vector to store values
        std::vector< std::vector<float> > farr(featureNum,
                                               std::vector<float>(0, 0));
        
        fs::directory_iterator end;
        for (fs::directory_iterator iter(directory);
             iter != end; iter++)
        {
            if (!quiet)
                std::cout << iter->path() << std::endl;

            if (fannDetector) {
                analyzeImage(fannDetector, features, iter->path().string());

                for (int i=0; i < featureNum; i++) {
                    farr[i].push_back(features[i]);
                }
            } else {
                processImage(symbolDetector, iter->path().string());
            }
        }

        // Sort the arrays
        for (int i=0; i < (int) farr.size(); i++) {
            std::sort(farr[i].begin(), farr[i].end());
        }
            
        for (int i=0; i < (int) farr.size(); i++) {
            std::cout << "Feature #" << i << std::endl;
            std::vector<float> flist = farr[i];
            std::cout << "Min: " << flist.front()
                      << "\nMax: " << flist.back() << std::endl;
                
            // Calculate the mean
            float mean = 0;
            BOOST_FOREACH(float f, flist)
            {
                mean += f;
            }
            mean /= flist.size();
            std::cout << "Mean: " << mean << std::endl;
                
            // Calculate the median
            float median = INFINITY;
            if (flist.size() % 2 == 0) {
                // Even
                median = (flist[flist.size()/2-1] + flist[flist.size()/2]) / 2;
            } else {
                // Odd
                median = flist[flist.size()/2];
            }
            std::cout << "Median: " << median << std::endl;
                
            // Calculate the standard deviation
            float stddev = 0;
            BOOST_FOREACH(float f, flist)
            {
                float diff = flist[i] - mean;
                stddev += diff * diff;
            }
            stddev /= flist.size() - 1;
            stddev = sqrt(stddev);
            std::cout << "Standard Deviation: " << stddev << std::endl;
                
            // TODO: Calculate these values for the trim
            int trim_start = (int) flist.size() * trim;
            int trim_end = (int) flist.size() - ((int) flist.size() * trim) - 1;
                
            std::cout << "Trim Min: " << flist[trim_start] << std::endl
                      << "Trim Max: " << flist[trim_end] << std::endl;
                
            double trim_mean = 0;
            for (int i=trim_start; i < trim_end; i++) {
                trim_mean += flist[i];
            }
            trim_mean /= trim_end - trim_start;
            std::cout << "Trim Mean: " << trim_mean << std::endl;
                
            float trim_stddev = 0;
            for (int i=trim_start; i < trim_end; i++) {
                float diff = flist[i] - trim_mean;
                trim_stddev += diff * diff;
            }
            trim_stddev /= trim_end - trim_start - 1;
            trim_stddev = sqrt(trim_stddev);
            std::cout << "Trim Standard Deviation: " << trim_stddev
                      << std::endl;
        }
    }

    delete[] features;

    return 0;
}

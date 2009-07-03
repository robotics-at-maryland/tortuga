/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/test/src/NetworkTrainer.cpp
 */

#include <iostream>
#include <vector>

#include <boost/filesystem/convenience.hpp>

#include "vision/include/Image.h"
#include "vision/include/FANNTrainer.h"
#include "vision/include/FANNSymbolDetector.h"
#include "vision/include/DetectorMaker.h"

#define NON_IMAGE_DIR_ARGS 3
#define MIN_OUTPUT_COUNT 2

namespace rv = ram::vision;

namespace ram {
    namespace NetworkTrainer {
        // function prototypes
        void usage();
        std::vector<rv::Image*> loadDirectory (boost::filesystem::path &dir);
    }
}

using namespace ram::NetworkTrainer;

int main (int argc, char * const argv[]) {
    // check for enough args
    if (argc < NON_IMAGE_DIR_ARGS + MIN_OUTPUT_COUNT) {
        usage();
        return 0;
    }
    // get the number of images and thier size
    std::string detectorName = std::string(argv[1]);
    // TODO load the config from somewhere
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString("{}"));

    // Create the detector
    config.set("type", detectorName);
    rv::DetectorPtr detector = rv::DetectorMaker::newObject(
        rv::DetectorMakerParamType(config, ram::core::EventHubPtr()));
    
    // Convert it the proper type
    rv::FANNSymbolDetectorPtr fannDetector = 
        boost::dynamic_pointer_cast<rv::FANNSymbolDetector>(detector);
    assert(fannDetector && "Symbol detector not of SymbolDetector type");
    
    // Check to make sure the user is supply enough input image directories
    int outputCount = fannDetector->getOutputCount();
    if (argc < (outputCount + NON_IMAGE_DIR_ARGS))
    {
        usage();
        return 0;
    }
            
    // build the testing network
    // TODO: try to load from file first, and the config information
    rv::FANNTrainer test(outputCount, fannDetector);
    
    // Load each set of training images, printing the index for each path
    // along the way
    FANN::training_data data;
    
    for (int dir = 0; dir < outputCount; ++dir) {
        std::vector<rv::Image*> imageList;
        std::string dirName(argv[dir + NON_IMAGE_DIR_ARGS]);
        boost::filesystem::path dirPath(dirName);

        if (boost::filesystem::basename (dirPath) != "")
        {
            imageList = loadDirectory(dirPath);
            test.addTrainData (dir, data, imageList);
            std::cout << "Directory: '" << dirName << "' assigned index: "
                      << dir << "\n";
            for (unsigned int i = 0; i < imageList.size(); ++i) {
                delete imageList[i];
            }
            imageList.clear();
        }
    }
    
    // print the original network
    test.print ();
    // train the network
    std::cout << "Beginning training.\n";
    test.runTraining (data);
    // print the trained network
    test.print ();
    // test the network
    std::cout << "Running a network test (using existing training data).\n";
    test.runTest (data);
    // save the network
    boost::filesystem::path savePath = boost::filesystem::path (argv[2]);
    test.save (savePath);
    // done
    std::cout << "Done\n";
    
    return 0;
}

std::vector<rv::Image*> ram::NetworkTrainer::loadDirectory (boost::filesystem::path &dir) {
    boost::filesystem::directory_iterator end;
    rv::Image* img = 0;
    std::vector<rv::Image*> imageList;
    if (!boost::filesystem::exists (dir)) {
        return imageList;
    }
    for (boost::filesystem::directory_iterator itr (dir); itr != end; ++itr) {
        if (boost::filesystem::basename (itr->path()) != "") {
        img = rv::Image::loadFromFile(itr->path().file_string());
        if (img) {
            // store the original image
            imageList.push_back (img);
        }
        }
    }
    return imageList;
}

// print usage
void ram::NetworkTrainer::usage () {
    std::cout << "Usage:\n";
    std::cout << "NetworkTrainer detectorName networkName trainingImages [trainingImages] ...\n";
    std::cout << "\tdetectorName:\tThe FANNSymbolDetector which will process the images\n";
    std::cout << "\tnetworkName:\tThe file in which to store the network.  If this file\n\t\talready exists it will "
              << "be opened and the network in it will be\n\t\ttrained further, if not it will be created and "
              << "saved in this\n\t\tfile.\n";
    std::cout << "\ttrainingImages:\tA path to a set of images on which to train the network.\n\t\tThere should be "
              << "one of these for each of the images to be\n\t\trecognized.  In other words, the number of these "
              << "should be\n\t\tequal to the value of 'images'.\n";
}

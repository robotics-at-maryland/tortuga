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
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

#include "vision/include/Image.h"
#include "vision/include/FANNTrainer.h"
#include "vision/include/FANNSymbolDetector.h"
#include "vision/include/DetectorMaker.h"

#define NON_IMAGE_DIR_ARGS 3
#define MIN_OUTPUT_COUNT 2

namespace po = boost::program_options;
namespace rv = ram::vision;

namespace ram {
    namespace NetworkTrainer {
        // function prototypes
        void usage();
        std::vector<rv::Image*> loadDirectory (boost::filesystem::path &dir);
    }
}

using namespace ram::NetworkTrainer;

int main (int argc, char** argv)
{
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    // Input file to read from
    std::vector<std::string> imageFileDirs;
    std::string fannDetectorType;
    std::string networkFileName;
    std::string configPath;
    bool inputDebug = true;
    
    try
    {
        // Positional Options
        p.add("detector", 1).
            add("network-file-name", 1).
            add("image-dirs", -1);

        // Option Descriptions
        desc.add_options()
            ("help", "Produce help message")
            ("debugOutput,d", po::bool_switch(&inputDebug),
             "Show the input produced by the training images")
            ("config,c", po::value<std::string>(&configPath)->
             default_value("NONE"), "Path to config with detector settings")
            ("detector", po::value<std::string>(&fannDetectorType),
             "FANN Detector to turn images into NN input")
            ("network-file-name", po::value<std::string>(&networkFileName),
             "Name of the file to save the network to")
            ("image-dirs", po::value< std::vector<std::string> >(&imageFileDirs),
             "Directories where the input images for the network come")
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
        std::cout << "Usage: NetworkTrainer [options] <detector> <network-name>"
                  << "<image-dirs>" << std::endl << desc << std::endl;
        
        return EXIT_FAILURE;
    }


    
    // check for enough args
    if (argc < NON_IMAGE_DIR_ARGS + MIN_OUTPUT_COUNT) {
        usage();
        return 0;
    }
    // get the number of images and thier size
    // TODO load the config from somewhere
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString("{}"));

    // Create the detector
    config.set("type", fannDetectorType);
    config.set("training", 1);
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
    rv::FANNTrainer test(fannDetector);
    
    // Load each set of training images, printing the index for each path
    // along the way
    FANN::training_data data;
    int index = 0;
    BOOST_FOREACH(std::string dirName, imageFileDirs)
    {
        std::vector<rv::Image*> imageList;
        boost::filesystem::path dirPath(dirName);

        if (boost::filesystem::basename (dirPath) != "")
        {
            imageList = loadDirectory(dirPath);
            bool result = test.addTrainData (index, data, imageList,
                                             inputDebug);
            if (!result)
            {
                std::cerr << "Error: with training: " << dirName << std::endl;
                return -1;
            }
            std::cout << "Directory: '" << dirName << "' assigned index: "
                      << index << " (" << imageList.size() << " images)\n";
            for (unsigned int i = 0; i < imageList.size(); ++i) {
                delete imageList[i];
            }
            imageList.clear();
        }

        index++;
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
    boost::filesystem::path savePath =
        boost::filesystem::path(networkFileName);
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

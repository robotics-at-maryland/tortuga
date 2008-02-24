/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/DetectorTest.cpp
 */

// STD Includes
#include <iostream>
#include <sstream>

// Library Includes
#include <highgui.h>
#include <boost/program_options.hpp>

// Project Includes
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Detector.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/ImageCamera.h"
#include "vision/include/FileRecorder.h"

#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"

namespace po = boost::program_options;
using namespace ram;

static const char* PROCESSED_WINDOW = "Processed Image";

int main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    // Input file to read from
    std::string input;
    std::string detectorName;
    std::string outputFilename;
    bool show = true;
    bool output = false;
    
    try
    {
        // Positional Options
        p.add("input", 1).
            add("detector", 1);

        // Option Descriptions
        desc.add_options()
            ("help", "produce help message")
            ("nogui,n", "suppress display of image")
            ("output,o", po::value<std::string>(&outputFilename),
             "file to write analyzed images to")
            ("input", po::value<std::string>(&input),
             "movie file")
            ("detector", po::value<std::string>(&detectorName)->
             default_value("RedLightDetector"), "Detector to run on the input")
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
        std::cout << "Usage: DetectorTest <input> [detector] [options]";

        // Loop through positional options and build the usage message          
        /*if (p.max_total_count() != 0)
        {
            std::cout << "[";
            for (size_t i = 0; i < p.max_total_count(); ++i)
                std::cout << " " << p.name_for_position(i);
            std::cout << "]";
        }
        */
        std::cout << std::endl;
        std::cout << desc << std::endl;
        
        return EXIT_FAILURE;
    }
    
    if (vm.count("nogui"))
        show = false;

    if (outputFilename.length() != 0)
        output = true;

    if (!(show || output))
    {
        std::cout << "Nothing to show on screen or write to disk, closing."
                  << std::endl;
        return EXIT_SUCCESS;
    }
    
    // Load the detector
    vision::DetectorPtr detector;
    
    if (vision::DetectorMaker::isKeyRegistered(detectorName))
    {
        // Generate a config with the proper type
        std::stringstream ss;
        ss << "{ 'type' : '" << detectorName << "'}";
        core::ConfigNode cfg(core::ConfigNode::fromString(ss.str()));
            
        detector = vision::DetectorMaker::newObject(
            vision::DetectorMakerParamType(cfg, core::EventHubPtr()));
    }
    else
    {
        std::cerr << "Detector '" << detectorName
                  << "' is not a valid detector" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Setup camera and OpenCV Window, and the Recorder
    vision::Camera* camera = new vision::OpenCVCamera(input.c_str());
    vision::Image* frame = new vision::OpenCVImage(camera->width(),
                                                   camera->height());
    vision::Image* outputImage = new vision::OpenCVImage(camera->width(),
                                                         camera->height());

    vision::ImageCamera* recordCamera = 0;
    vision::Recorder* recorder = 0;

    if (show)
        cvNamedWindow(PROCESSED_WINDOW, CV_WINDOW_AUTOSIZE);
    
    // Main Loop
    while(1)
    {
        // Get one frame and process
        camera->update(1.0 / camera->fps());
        camera->getImage(frame);
        detector->processImage(frame, outputImage);

        if (output)
        {
            // Create recorder if we haven't already
            // This needs to be done here because we can't be sure of the size
            // of the output image isn't know till now
            if (!recorder)
            {
                recordCamera = new vision::ImageCamera(
                    outputImage->getWidth(), outputImage->getHeight(),
                    camera->fps());
                recorder = new vision::FileRecorder(
                    recordCamera, vision::Recorder::NEXT_FRAME, outputFilename);
                recorder->unbackground(true);
            }
            
            recordCamera->newImage(outputImage);
            recorder->update(1.0/recordCamera->fps());
        }
        
        if (show)
            cvShowImage(PROCESSED_WINDOW, outputImage->asIplImage());

        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits using AND operator
        if (show)
        {
            if ((cvWaitKey(1) & 255) == 27)
                break;
        }
    }


    // Clean up
    if (show)
        cvDestroyWindow(PROCESSED_WINDOW);

    delete camera;
    delete recorder;

    delete recordCamera;

    delete frame;
    delete outputImage;
}

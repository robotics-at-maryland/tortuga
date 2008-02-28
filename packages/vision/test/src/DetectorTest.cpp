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
#include <signal.h>

// Library Includes
#include <highgui.h>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

// Project Includes
#include "vision/include/OpenCVCamera.h"
#include "vision/include/NetworkCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Detector.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/ImageCamera.h"
#include "vision/include/FileRecorder.h"
#include "vision/include/NetworkRecorder.h"

#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"

namespace po = boost::program_options;
using namespace ram;

static const char* PROCESSED_WINDOW = "Processed Image";

/** Creates the camera based on the input stream */
vision::Camera* createCamera(std::string input);
vision::Recorder* createRecorder(std::string output, vision::Camera* camera);
void brokenPipeHandler(int signum);
int main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    // Input file to read from
    std::string input;
    std::string detectorName;
    std::string output;
    bool show = true;
    bool outputing = false;
    bool runDetector = false;
    
    try
    {
        // Positional Options
        p.add("input", 1).
            add("detector", 1);

        // Option Descriptions
        desc.add_options()
            ("help", "Produce help message")
            ("nogui,n", po::bool_switch(&show),
             "Suppress display of image")
            ("disable-detector,d", po::bool_switch(&runDetector),
             "Do not run the detector on input")
            ("output,o", po::value<std::string>(&output),
             "File or network port to send images to")
            ("input", po::value<std::string>(&input),
             "Video file, camera #, hostname:port")
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
        std::cout << "Usage: DetectorTest <input> [detector] [options]"
                  << std::endl << desc << std::endl;
        
        return EXIT_FAILURE;
    }
    
    // Must swap values, because boost.programoptions stores the negation
    show = !show;
    runDetector = !runDetector;

    if (output.length() != 0)
        outputing = true;

    if (!(show || outputing))
    {
        std::cout << "Nothing to show on screen or write to disk, closing."
                  << std::endl;
        return EXIT_SUCCESS;
    }
    
    // Load the detector
    vision::DetectorPtr detector;

    if (runDetector)
    {
        if (vision::DetectorMaker::isKeyRegistered(detectorName))
        {
            // Generate a config with the proper type
            std::stringstream ss;
            ss << "{ 'type' : '" << detectorName << "'}";
            core::ConfigNode cfg(core::ConfigNode::fromString(ss.str()));
            
            detector = vision::DetectorMaker::newObject(
                vision::DetectorMakerParamType(cfg, core::EventHubPtr()));
            std::cout << "Running '" << detectorName << "' on input images"
                      << std::endl;
        }
        else
        {
            std::cerr << "Detector '" << detectorName
                      << "' is not a valid detector" << std::endl;
            return EXIT_FAILURE;
        }
    }
    
    // Setup camera and OpenCV Window, and the Recorder
    vision::Camera* camera = createCamera(input);
    vision::Image* frame = new vision::OpenCVImage(camera->width(),
                                                   camera->height());
    vision::Image* outputImage = new vision::OpenCVImage(camera->width(),
                                                         camera->height());

    vision::ImageCamera* recordCamera = 0;
    vision::Recorder* recorder = 0;

    if (show)
    {
        cvNamedWindow(PROCESSED_WINDOW, CV_WINDOW_AUTOSIZE);

        if (runDetector)
            std::cout << "Showing processed";
        else
            std::cout << "Showing input";
        
        std::cout << " images on window '" << PROCESSED_WINDOW << "'"
                  << std::endl;
    }

    // Main Loop
    while(1)
    {
        vision::Image* workingImage = frame;
        
        // Get one frame and process
        camera->update(1.0 / camera->fps());
        camera->getImage(frame);

        if (runDetector)
        {
            detector->processImage(frame, outputImage);
            workingImage = outputImage;
        }

        if (outputing)
        {
            // Create recorder if we haven't already
            // This needs to be done here because we can't be sure of the size
            // of the output image isn't know till now
            if (!recorder)
            {
                std::cout << "Writing images to " << output << std::endl;
                
                recordCamera = new vision::ImageCamera(
                    workingImage->getWidth(), workingImage->getHeight(),
                    camera->fps());
                                    
                recorder = createRecorder(output, recordCamera);
                recorder->unbackground(true);
            }
            
            recordCamera->newImage(workingImage);
            recorder->update(1.0/recordCamera->fps());
        }
        
        if (show)
            cvShowImage(PROCESSED_WINDOW, workingImage->asIplImage());

        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits using AND operator
        if (show)
        {
            if ((cvWaitKey(10) & 255) == 27)
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

vision::Recorder* createRecorder(std::string output, vision::Camera* camera)
{
    static boost::regex port("(\\d{1,5})");
    
    boost::smatch matcher;
    if (boost::regex_match(output, matcher, port))
    {
        std::cout <<"Recording to host : '" << boost::lexical_cast<int>(output)
                  << "'" << std::endl;
        boost::uint16_t portNum = boost::lexical_cast<boost::uint16_t>(output);
        
        signal(SIGPIPE,brokenPipeHandler);
        vision::Recorder* r =
            new vision::NetworkRecorder(camera, vision::Recorder::NEXT_FRAME, portNum);
        return r;
    }

    std::cout <<"Assuming output is a file, Recording to '" << output << "'"
              << std::endl;
    return new vision::FileRecorder(camera, vision::Recorder::NEXT_FRAME, output);
}

void brokenPipeHandler(int signum)
{
    std::cout<<"Broken Pipe Ignored"<<std::endl;
}

vision::Camera* createCamera(std::string input)
{
    static boost::regex camnum("\\d+");
    static boost::regex hostnamePort("([a-zA-Z0-9.-_]+):(\\d{1,5})");

    std::cout << "Images coming from: ";
    
    if (boost::regex_match(input, camnum))
    {
        int camnum = boost::lexical_cast<int>(input);
        std::cout << "Forward Camera #" << camnum << std::endl;
        return new vision::OpenCVCamera(camnum, true);
    }

    boost::smatch what;
    if (boost::regex_match(input, what, hostnamePort))
    {
        std::string hostname = what.str(1);
        boost::uint16_t port =
            boost::lexical_cast<boost::uint16_t>(what.str(2));
        std::cout << "Streaming images from host: \"" << hostname
                  << "\" on port " << port << std::endl;
        return new vision::NetworkCamera(hostname, port);
    }

    std::cout << "'" << input << "' video file" << std::endl;
    return new vision::OpenCVCamera(input);
}

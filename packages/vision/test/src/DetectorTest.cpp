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
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/ImageCamera.h"
#include "vision/include/Image.h"
#include "vision/include/NetworkCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Detector.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/ImageCamera.h"
#include "vision/include/FileRecorder.h"
#include "vision/include/FFMPEGRecorder.h"
#include "vision/include/NetworkRecorder.h"
#include "vision/include/FFMPEGNetworkRecorder.h"
#include "vision/include/FFMPEGNetworkCamera.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/DC1394Camera.h"

#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

namespace po = boost::program_options;
using namespace ram;

static const char* PROCESSED_WINDOW = "Processed Image";

/** Creates a recorder based on the input stream */
vision::Recorder* createRecorder(std::string output, vision::Camera* camera);

/** Creates a dector of the given type, tries to use given config file */
vision::DetectorPtr createDetector(std::string dectorType,
                                   std::string configPath);

/** Searches all sections in the config for one which has the given type */
vision::DetectorPtr createDetectorFromConfig(std::string detectorType,
                                             core::ConfigNode cfg,
                                             std::string& nodeUsed);

/** Print out all the detectors properties and values */
void dumpDetectorProperties(vision::DetectorPtr detector);

// Handle a corner case on Mac OSX
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
    std::string configPath;
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
             "Video/Image file, camera # (>= 100 == firewire), hostname:port")
            ("detector", po::value<std::string>(&detectorName)->
             default_value("RedLightDetector"), "Detector to run on the input")
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
        detector = createDetector(detectorName, configPath);

        if (detector)
        {
            std::cout << "Running '" << detectorName << "' on input images"
                      << std::endl;
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    
    // Setup camera and OpenCV Window, and the Recorder
    std::string message;
    vision::CameraPtr camera = vision::Camera::createCamera(input, configPath,
                                                          message);
    std::cout << message << std::endl;
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
    bool inputing = true;
    while(1)
    {
        vision::Image* workingImage = frame;
        
        // Get one frame and process
        if (inputing)
        {
            camera->update(1.0 / camera->fps());
            camera->getImage(frame);
        }

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
		{
            cvShowImage(PROCESSED_WINDOW, workingImage->asIplImage());
		}
        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits using AND operator
        if (show)
        {
            char key;
            if ((key = (char)(cvWaitKey(10) & 255)) == 27)
                break;
                
            if (key == 't' && detector)
            {
                runDetector = !runDetector;
            }

            if (key == 'p')
            {
                inputing = !inputing;
            }
        }
    }


    // Clean up
    if (show)
        cvDestroyWindow(PROCESSED_WINDOW);

    camera = vision::CameraPtr();
    delete recorder;

    delete recordCamera;

    delete frame;
    delete outputImage;
}

vision::DetectorPtr createDetector(std::string detectorType,
                                   std::string configPath)
{
    // Bail out early if there is no such dectector
    if (!vision::DetectorMaker::isKeyRegistered(detectorType))
    {
        std::cerr << "Detector '" << detectorType
                  << "' is not a valid detector" << std::endl;
        return vision::DetectorPtr();
    }
    
    vision::DetectorPtr detector;
    
    // Generate a config with the proper type
    if ("NONE" != configPath)
    {
        std::string nodeUsed;
        core::ConfigNode cfg(core::ConfigNode::fromFile(configPath));

        // Attempt to find at the base level
        detector = createDetectorFromConfig(detectorType, cfg, nodeUsed);

        if (!detector)
        {
            // Attempt to find the section deeper in the file
            if (cfg.exists("Subsystems"))
            {
                cfg = cfg["Subsystems"];
                        
                // Attempt to find a VisionSystem subsystem
                core::NodeNameList nodeNames(cfg.subNodes());
                BOOST_FOREACH(std::string nodeName, nodeNames)
                {
                    core::ConfigNode subsysCfg(cfg[nodeName]);
                    if (("VisionSystem" == subsysCfg["type"].asString("NONE"))||
                        ("SimVision" == subsysCfg["type"].asString("NONE")))
                    {
                        std::cout << "Looking in config section" << std::endl;
                        // Attempt to find in the list of detectors
                        detector =
                            createDetectorFromConfig(detectorType,
                                                     subsysCfg,
                                                     nodeUsed);

                        std::stringstream ss;
                        ss << "Subsystem:" << nodeName << ":" << nodeUsed;
                        nodeUsed = ss.str();
                    }
                }
            }
        }

        if (detector)
        {
            std::cout << "Using section \"" << nodeUsed << "\" for detector \""
                      << detectorType << "\"" << std::endl << "from config "
                      << "file: \"" << configPath << "\"" << std::endl;
            dumpDetectorProperties(detector);
        }
        else
        {
            std::cerr << "Cannot find config information for dectector '"
                      << detectorType << "'" << std::endl << " in file: \""
                      << configPath << "\"" << std::endl;
        }

        return detector;
    }
    else
    {
	std::cout << "---No Config File Specified!--- Using Default Values"<<std::endl;
        std::stringstream ss;
        ss << "{ 'type' : '" << detectorType << "'}";
        core::ConfigNode cfg(core::ConfigNode::fromString(ss.str()));
        
        detector = vision::DetectorMaker::newObject(
            vision::DetectorMakerParamType(cfg, core::EventHubPtr()));
    }

    dumpDetectorProperties(detector);
    return detector;
}

vision::DetectorPtr createDetectorFromConfig(std::string detectorType,
                                             core::ConfigNode cfg,
                                             std::string& nodeUsed)
{
    core::NodeNameList nodeNames(cfg.subNodes());
    // Go through each section and check its type
    BOOST_FOREACH(std::string nodeName, nodeNames)
    {
        core::ConfigNode cfgSection(cfg[nodeName]);
        if ((detectorType == cfgSection["type"].asString("NONE"))
             || (nodeName == detectorType))
        {
            nodeUsed = nodeName;
            cfgSection.set("type", detectorType);
            return vision::DetectorMaker::newObject(
                vision::DetectorMakerParamType(cfgSection,
                                               core::EventHubPtr()));
        }
    }
    
    return vision::DetectorPtr();
}

void dumpDetectorProperties(vision::DetectorPtr detector)
{
    core::PropertySetPtr propSet(detector->getPropertySet());
    std::vector<std::string> propNames = propSet->getPropertyNames();
    if (propNames.size() > 0)
    {
        std::cout << "Detector has the following properties:" << std::endl;
        BOOST_FOREACH(std::string propName, propNames)
        {
            core::PropertyPtr prop(propSet->getProperty(propName));
            std::cout << "\t" << propName << ": " << prop->toString()
                      << std::endl;
        }
    }
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
#ifdef RAM_POSIX
        signal(SIGPIPE, brokenPipeHandler);
#endif
        vision::Recorder* r =
//            new vision::NetworkRecorder(camera, vision::Recorder::NEXT_FRAME, portNum);
            new vision::FFMPEGNetworkRecorder(camera, vision::Recorder::NEXT_FRAME, portNum);
        return r;
    }

    std::cout <<"Assuming output is a file, Recording to '" << output << "'"
              << std::endl;
    return new vision::FFMPEGRecorder(camera, vision::Recorder::NEXT_FRAME, output);
}

void brokenPipeHandler(int signum)
{
    std::cout<<"Broken Pipe Ignored"<<std::endl;
}

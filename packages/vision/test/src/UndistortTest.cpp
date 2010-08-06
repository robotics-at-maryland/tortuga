/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/UndistortTest.cpp
 */

// STD Includes
#include <iostream>
#include <signal.h>

// Library Includes
#include "cv.h"
#include "highgui.h"

#include <boost/program_options.hpp>
#include <boost/regex.hpp>

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Recorder.h"
#include "vision/include/ImageCamera.h"
#include "vision/include/FFMPEGRecorder.h"
#include "vision/include/FFMPEGNetworkRecorder.h"
#include "vision/include/FFMPEGNetworkCamera.h"

using namespace ram;
namespace po = boost::program_options;

static const char* PROCESSED_WINDOW = "Processed Image";

void brokenPipeHandler(int signum)
{
    std::cout<<"Broken Pipe Ignored"<<std::endl;
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

int main(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    std::string input;
    std::string intrinsicsFile;
    std::string distortionFile;
    std::string configPath;
    std::string output;

    try {
        p.add("input", 1).
            add("intrinsics", 1).
            add("distortion", 1);

        desc.add_options()
            ("help", "Produce help message")
            ("input", po::value<std::string>(&input),
             "Video/Image file, camera # (>= 100 == firewire), hostname:port")
            ("intrinsics,i", po::value<std::string>(&intrinsicsFile),
             "Intrinsics file for distortion map")
            ("distortion,d", po::value<std::string>(&distortionFile),
             "Distortion file for distortion map")
            ("output,o", po::value<std::string>(&output),
             "File or network port to send images to")
            ("config,c", po::value<std::string>(&configPath)->
             default_value("NONE"), "Path to config with detector settings")
            ;

        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);
    } catch(std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help")) {
        std::cout << "Usage: UndistortTest <input> [options]"
                  << std::endl << desc << std::endl;

        return EXIT_FAILURE;
    }

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

    CvMat *intrinsic = (CvMat*)cvLoad( intrinsicsFile.c_str() );
    CvMat *distortion = (CvMat*)cvLoad( distortionFile.c_str() );

    IplImage* mapx = cvCreateImage( cvGetSize( frame->asIplImage() ),
                                    IPL_DEPTH_32F, 1 );
    IplImage* mapy = cvCreateImage( cvGetSize( frame->asIplImage() ),
                                    IPL_DEPTH_32F, 1 );
    cvInitUndistortMap( intrinsic, distortion, mapx, mapy );

    cvNamedWindow(PROCESSED_WINDOW, CV_WINDOW_AUTOSIZE);

    while(1) {
        vision::Image* workingImage = frame;

        camera->update(1.0 / camera->fps());
        camera->getImage(frame);

        // Undistort the image
        cvRemap( frame->asIplImage(), outputImage->asIplImage(), mapx, mapy );
        cvShowImage( PROCESSED_WINDOW, outputImage->asIplImage() );

        if (output.length() != 0) {
            if (!recorder) {
                std::cout << "Writing images to " << output << std::endl;
                
                recordCamera = new vision::ImageCamera(
                    workingImage->getWidth(), workingImage->getHeight(),
                    camera->fps());
                
                recorder = createRecorder(output, recordCamera);
                recorder->unbackground(true);
            }

            recordCamera->newImage(outputImage);
            recorder->update(1.0/recordCamera->fps());
        }

        char key;
        if ((key = (char)(cvWaitKey(10) & 255)) == 27)
            break;
    }

    cvDestroyWindow(PROCESSED_WINDOW);

    delete frame;
    delete outputImage;
}


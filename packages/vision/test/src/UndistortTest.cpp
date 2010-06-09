/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/UndistortTest.cpp
 */

// Library Includes
#include "cv.h"
#include "highgui.h"

#include <boost/program_options.hpp>

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"

using namespace ram;
namespace po = boost::program_options;

static const char* PROCESSED_WINDOW = "Processed Image";

int main(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    std::string input;
    std::string intrinsicsFile;
    std::string distortionFile;
    std::string configPath;

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

    CvMat *intrinsic = (CvMat*)cvLoad( intrinsicsFile.c_str() );
    CvMat *distortion = (CvMat*)cvLoad( distortionFile.c_str() );

    IplImage* mapx = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 1 );
    IplImage* mapy = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 1 );
    cvInitUndistortMap( intrinsic, distortion, mapx, mapy );

    cvNamedWindow(PROCESSED_WINDOW, CV_WINDOW_AUTOSIZE);

    while(1) {
        vision::Image* workingImage = frame;

        camera->update(1.0 / camera->fps());
        camera->getImage(frame);

        // Undistort the image
        cvRemap( frame->asIplImage(), outputImage->asIplImage(), mapx, mapy );
        cvShowImage( PROCESSED_WINDOW, outputImage );

        char key;
        if ((key = (char)(cvWaitKey(10) & 255)) == 27)
            break;
    }

    cvDestroyWindow(PROCESSED_WINDOW);

    delete frame;
    delete outputImage;
}


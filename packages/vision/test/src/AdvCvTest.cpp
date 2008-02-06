/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/CvCamTest.cpp
 */

// Library Includes
#include <highgui.h>

// Project Includes
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"

const static int FPS = 10;

// A Simple OpenCV based test of the Vision System
int main() {
    ram::vision::OpenCVCamera* camera = new ram::vision::OpenCVCamera();
    ram::vision::Image* frame = \
        //      new ram::vision::OpenCVImage(camera->width(), camera->height());
        new ram::vision::OpenCVImage(640, 480);
        
    // Start camera updating in background
    camera->background(); // Run as fast as the camera provides images
    
    // Create a window in which the captured images will be presented
    cvNamedWindow( "Raw Camera Image", CV_WINDOW_AUTOSIZE );

    // Show the image captured from the camera in the window and repeat
    while( 1 ) {
        // Waits for the background thread to grab and image, then gets the
        // image
        camera->waitForImage(frame);
        //camera->update(0);
        //camera->getImage(frame);
 
        // The case is need because cvShowImage basically takes a void*,
        // otherwise you would just need (*frame)
        cvShowImage( "Raw Camera Image", (IplImage*)(*frame) );
        
        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits  using AND operator
        if( (cvWaitKey(10) & 255) == 27 ) break;
    }

    // Release the capture device housekeeping
    delete frame;
    delete camera;
    cvDestroyWindow("Raw Camera Image");
    return 0;
}

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
#include "vision/include/Image.h"

// A Simple OpenCV based test of the Vision System
int main() {
    ram::vision::OpenCVCamera* camera = new ram::vision::OpenCVCamera();

    // Create a window in which the captured images will be presented
    cvNamedWindow( "Raw Camera Image", CV_WINDOW_AUTOSIZE );

    // Show the image captured from the camera in the window and repeat
    while( 1 ) {
        ram::vision::Image* frame = camera->getImage();
        
        cvShowImage( "Raw Camera Image", frame );
        // Do not release the frame!
        
        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits  using AND operator
        if( (cvWaitKey(10) & 255) == 27 ) break;
    }

  // Release the capture device housekeeping
  delete camera;
  cvDestroyWindow( "mywindow" );
  return 0;
}

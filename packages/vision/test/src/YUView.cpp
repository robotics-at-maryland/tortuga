/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/CvCameraTest.cpp
 */


// STD Includes
#include <stdlib.h>
#include <stdio.h>

// Library Includes
#include "cv.h"
#include "highgui.h"

static char* UV_FRAME_NAME = "UV part of YUV in RGB";
static char* Y_FRAME_NAME = "Y part of YUV in Grayscale";
static char* U_FRAME_NAME = "U part of YUV in Grayscale";
static char* V_FRAME_NAME = "V part of YUV in Grayscale";

struct Images
{
    IplImage* src;
    IplImage* yuv;
    IplImage* display;
    IplImage* ydisplay;
    IplImage* udisplay;
    IplImage* vdisplay;
};

void usageError()
{
    fprintf(stderr, "Usage: CvCameraTest [camera number or movie file]\n");
    getchar();
    exit(EXIT_FAILURE);
}

void updateImages(Images* images)
{
    // Transform source to YUV
    cvCvtColor(images->src, images->yuv, CV_BGR2YCrCb);

    // Remove luminance from the YUV (the 'Y' channel)
    unsigned char* dataYUV = (unsigned char*)images->yuv->imageData;
    unsigned char* dataY = (unsigned char*)images->ydisplay->imageData;
    unsigned char* dataU = (unsigned char*)images->udisplay->imageData;
    unsigned char* dataV = (unsigned char*)images->vdisplay->imageData;

    unsigned char Y;
    unsigned char U;
    unsigned char V;
    int length = images->yuv->height * images->yuv->width * 3;
    for (int pos = 0; pos < (length - 1); pos += 3)
    {       
        // Build grayscale Y, U, and V images
        Y = dataYUV[pos];
        U = dataYUV[pos + 1];
        V = dataYUV[pos + 2];

        dataY[pos] = Y;
        dataY[pos + 1] = Y;
        dataY[pos + 2] = Y;
        
        dataU[pos] = U;
        dataU[pos + 1] = U;
        dataU[pos + 2] = U;
        
        dataV[pos] = V;
        dataV[pos + 1] = V;
        dataV[pos + 2] = V;

        // Eliminate Y data from YUV
        dataYUV[pos] = 0;
    }

    // Copy the YUV out for display
    cvCvtColor(images->yuv, images->display, CV_YCrCb2BGR);
}

int main(int argc, char** argv)
{
    int camNum = CV_CAP_ANY;
    CvCapture* capture = 0;
    char* tailPtr = (char*)1;
        
    // Parse arguments and open camera
    if (argc == 2)
    {
        if ((0 == strcmp(argv[1], "--help")) || (0 == strcmp(argv[1], "-h")))
            usageError();
        
        camNum = (int)strtol(argv[1], &tailPtr, 10);
        
        if ((*tailPtr) != '\0')
        {
            if (NULL == (capture = cvCaptureFromFile(argv[1])))
            {
                fprintf(stderr, "ERROR: capture is NULL, can't open movie file"
                        " '%s'\n", argv[1]);
                getchar();
                return EXIT_FAILURE;
            }
        }
        else
        {
            capture = cvCaptureFromCAM( CV_CAP_ANY );
            fprintf(stderr, "ERROR: capture is NULL, no camera with num '%d'\n",
                    camNum);
            getchar();
            return EXIT_FAILURE;
        }
    }
    else if (argc != 1)
    {
        usageError();
    }

    
    printf("Camera Capture Properties:\n");
    printf("\tWidth:  %f\n", cvGetCaptureProperty(capture,
                                                  CV_CAP_PROP_FRAME_WIDTH));
    printf("\tHeight: %f\n", cvGetCaptureProperty(capture,
                                                  CV_CAP_PROP_FRAME_HEIGHT));
    printf("\tFPS:    %f\n", cvGetCaptureProperty(capture,
                                                  CV_CAP_PROP_FPS));
    
    // Create a window in which the captured images will be presented
    cvNamedWindow(UV_FRAME_NAME, CV_WINDOW_AUTOSIZE);
    cvNamedWindow(Y_FRAME_NAME, CV_WINDOW_AUTOSIZE);
    cvNamedWindow(U_FRAME_NAME, CV_WINDOW_AUTOSIZE);
    cvNamedWindow(V_FRAME_NAME, CV_WINDOW_AUTOSIZE);

    // Manual window position for easy display of all for windows
    /*cvMoveWindow(UV_FRAME_NAME, 1670 + 0, 0);
    cvMoveWindow(Y_FRAME_NAME, 1670 + 400, 0);
    cvMoveWindow(U_FRAME_NAME, 1670 + 400, 400);
    cvMoveWindow(V_FRAME_NAME, 1670 + 0, 400);*/

    /*cvMoveWindow(UV_FRAME_NAME, 1670 + 0, 0);
    cvMoveWindow(Y_FRAME_NAME, 1670 + 670, 0);
    cvMoveWindow(U_FRAME_NAME, 1670 + 670, 515);
    cvMoveWindow(V_FRAME_NAME, 1670 + 0, 515);*/
    
    Images images;
    images.src = 0;
    images.yuv = 0;
    images.display = 0;
    
    // Show the image captured from the camera in the window and repeat
    while(1) {
        // Get one frame
        IplImage* frame = cvQueryFrame(capture);

        if (!frame) 
        {
            fprintf(stderr,
                    "ERROR: frame is null, could not get frame from camera\n");
            break;
        }

        // Update images to show just the UV part of the video
        if (0 == images.yuv)
        {
            images.yuv = cvCreateImage(cvGetSize(frame), 8, 3);
            images.display = cvCreateImage(cvGetSize(frame), 8, 3);
            images.ydisplay = cvCreateImage(cvGetSize(frame), 8, 3);
            images.udisplay = cvCreateImage(cvGetSize(frame), 8, 3);
            images.vdisplay = cvCreateImage(cvGetSize(frame), 8, 3);
        }
        images.src = frame;
        updateImages(&images);
            
        
        cvShowImage(UV_FRAME_NAME, images.display);
        cvShowImage(Y_FRAME_NAME, images.ydisplay);
        cvShowImage(U_FRAME_NAME, images.udisplay);
        cvShowImage(V_FRAME_NAME, images.vdisplay);

        //Do not release the frame!
        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits using AND operator
        if ((cvWaitKey(10) & 255) == 27)
            break;
    }
    
    // Release the capture device housekeeping
    cvReleaseCapture(&capture);
    cvReleaseImage(&images.yuv);
    cvReleaseImage(&images.display);
    cvDestroyWindow(UV_FRAME_NAME);
    cvDestroyWindow(Y_FRAME_NAME);
    cvDestroyWindow(U_FRAME_NAME);
    cvDestroyWindow(V_FRAME_NAME);
    
    return EXIT_SUCCESS;
}

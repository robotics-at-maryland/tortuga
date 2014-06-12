/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/YUVColorBox.cpp
 */

// STD Includes
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>

// Library Includes
#include "cv.h"
#include "highgui.h"

static char* WINDOW_NAME = "YUV Color Space";
static char* TRACKBAR_NAME = "Y Color Value";
static CvSize IMAGE_SIZE = cvSize(640, 640);

struct Images
{
    IplImage* yuv;
    IplImage* rgb;
    IplImage* display;
};

// color conversion functions from Bart Nabbe.
// corrected by Damien: bad coeficients in YUV2RGB
#define YUV2RGB(y, u, v, r, g, b) {\
  r = y + ((v*1436) >> 10);\
  g = y - ((u*352 + v*731) >> 10);\
  b = y + ((u*1814) >> 10);\
  r = r < 0 ? 0 : r;\
  g = g < 0 ? 0 : g;\
  b = b < 0 ? 0 : b;\
  r = r > 255 ? 255 : r;\
  g = g > 255 ? 255 : g;\
  b = b > 255 ? 255 : b; }

#define RGB2YUV(r, g, b, y, u, v) {\
  y = (306*r + 601*g + 117*b)  >> 10;\
  u = ((-172*r - 340*g + 512*b) >> 10)  + 128;\
  v = ((512*r - 429*g - 83*b) >> 10) + 128;\
  y = y < 0 ? 0 : y;\
  u = u < 0 ? 0 : u;\
  v = v < 0 ? 0 : v;\
  y = y > 255 ? 255 : y;\
  u = u > 255 ? 255 : u;\
  v = v > 255 ? 255 : v; }

void fillYUVImage(IplImage* image, int Y);

void mouseCallback(int event, int x, int y, int flags, void* param);

void gradientFill(IplImage* image);

void updateImages(Images* images, int YValue)
{
    fillYUVImage(images->yuv, YValue);
    cvCvtColor(images->yuv, images->rgb, CV_YCrCb2BGR);
    cvResize(images->rgb, images->display);
}

int main()
{
    Images images;
    images.yuv = cvCreateImage(IMAGE_SIZE, 8, 3);
    images.rgb = cvCreateImage(IMAGE_SIZE, 8, 3);
    images.display = cvCreateImage(IMAGE_SIZE, 8, 3);
    
    int YValue = 255/2;
    int oldYValue = YValue;

    updateImages(&images, YValue);
    
    // Create GUI Controls
    cvNamedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    cvCreateTrackbar(TRACKBAR_NAME, WINDOW_NAME, &YValue, 255, NULL);

    // Set up printing mouse callback
    cvSetMouseCallback(WINDOW_NAME, mouseCallback, &images);
    
    // Show the image captured from the camera in the window and repeat
    while(1) {
        cvShowImage(WINDOW_NAME, images.display);

        // Read trackbar state and only update on change
        YValue = cvGetTrackbarPos(TRACKBAR_NAME, WINDOW_NAME);
        if (YValue != oldYValue)
        {
            updateImages(&images, YValue);
            oldYValue = YValue;
        }
        
        //Do not release the frame!
        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits using AND operator
        if ((cvWaitKey(10) & 255) == 27)
            break;
    }
    
    // Release the frames housekeeping
    cvReleaseImage(&images.yuv);
    cvReleaseImage(&images.rgb);
    cvReleaseImage(&images.display);
    cvDestroyWindow(WINDOW_NAME);
    
    return EXIT_SUCCESS;
}

void fillYUVImage(IplImage* image, int Y)
{
    unsigned char* data = (unsigned char*)image->imageData;
    int pos = 0;
    double divisorRow = image->height / 255.0;
    double divisorCol = image->width / 255.0;
    
    // The fill is drawn with an odd diagonal
    for (int row = 0; row < image->height; ++row)
    {
        for (int col = 0; col < image->width; ++col)
        {
            unsigned char U = (unsigned char) (col / divisorCol);
            unsigned char V = (unsigned char) (row / divisorRow);
            data[pos] = Y;     // Y
            data[pos + 1] = U; // U
            data[pos + 2] = V; // V

            pos += 3;
        }
    }
}

void mouseCallback(int event, int x, int y, int flags, void* param)
{
    if (CV_EVENT_LBUTTONDOWN == event)
    {
        Images* images = (Images*)param;
        unsigned char* yuvData = (unsigned char*)images->yuv->imageData;
        unsigned char* rgbData = (unsigned char*)images->rgb->imageData;
        int offset = (y * images->display->width * 3) + (x * 3);
        
        int R, G, B;
        int Y, U, V;

        B = (uint8_t)rgbData[offset];
        G = (uint8_t)rgbData[offset + 1];
        R = (uint8_t)rgbData[offset + 2];

        Y = (uint8_t)yuvData[offset];
        U = (uint8_t)yuvData[offset + 1];
        V = (uint8_t)yuvData[offset + 2];

//        RGB2YUV(R, G, B, Y, U, V);

        double divisor = images->display->width / 255.0;
        uint8_t xPos = std::min((uint8_t) (x / divisor), (uint8_t)255u);
        divisor = images->display->height / 255.0;
        uint8_t yPos = std::min((uint8_t) (y / divisor), (uint8_t)255u);
        
        // Print out current pixel data
        printf("Pos (%3d,%3d) => Y: %3d U: %3d V: %3d | "
               "R: %3d G: %3d B: %3d\n", xPos, yPos, Y, U, V, R, G, B);
    }
}

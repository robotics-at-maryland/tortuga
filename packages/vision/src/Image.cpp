/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Image.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/OpenCVImage.h"

#include "math/include/Matrix3.h"

static const char* DEBUG_WINDOW = "Debug Image (close w/ESC Key)";

namespace ram {
namespace vision {

Image* Image::loadFromFile(std::string fileName)
{
    IplImage* image = cvLoadImage(fileName.c_str());
    if (image)
	{
        return new OpenCVImage(image, true);
	}
	else
	{
		std::cout << "Could not load file: \"" << fileName << "\"" << std::endl;
		assert(false && "Could not load image");
	}
    return 0;
}

void Image::saveToFile(Image* image, std::string fileName)
{
    cvSaveImage(fileName.c_str(), image->asIplImage());
}

void Image::transform(Image* src, Image* dest, math::Degree rotation,
                      double scale, int xTrans, int yTrans,
                      unsigned char fillR, unsigned char fillG,
                      unsigned char fillB)
{
    // Create translation matrix
    math::Matrix3 translate(1, 0, xTrans,
                            0, 1, yTrans,
                            0, 0, 1);

    // Create rotation Matrix
    math::Matrix3 rotate(0, 0, 0,
                         0, 0, 0,
                         0, 0, 1.0);

    CvMat R = cvMat(2, 3, CV_64F, rotate[0]);
    CvPoint2D32f center = {dest->getWidth()/2, dest->getHeight()/2};
    cv2DRotationMatrix(center, -rotation.valueDegrees(), scale, &R);

    // Combine translation and rotation
    math::Matrix3 result = translate * rotate;
    CvMat M = cvMat(3, 3, CV_64F, result[0]);    
    
    // Change the image (what a crazy function to use for this)
    cvWarpPerspective(src->asIplImage(), dest->asIplImage(),
                      &M, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS,
                      CV_RGB(fillB, fillG, fillR));
}

void Image::blitImage(Image* toBlit, Image* src, Image* dest,
                      unsigned char R, unsigned char G, unsigned char B,
                      int xOffset, int yOffset)
{
    size_t toBlitBytes = toBlit->getWidth() * toBlit->getHeight() * 3;
    size_t destBytes = dest->getWidth() * dest->getHeight() * 3;
    size_t srcBytes = src->getWidth() * src->getHeight() * 3;
    assert((destBytes == toBlitBytes) && (toBlitBytes == srcBytes) &&
           "Images are not the same size");

    // Preform a full copy of src, to dest
    cvCopy(src->asIplImage(), dest->asIplImage());

    // Now do the blit
    unsigned char* blitData = toBlit->getData();
    unsigned char* destData = dest->getData();

    unsigned char* blitStart = blitData;
    unsigned char* destStart = destData;
    
    int width = dest->getWidth();
    int height = dest->getHeight();

    for (int y = yOffset; y < height; ++y)
    {
        // Select proper row of blit data (without the offset)
        blitData = blitStart + (width * (y - yOffset) * 3);
        
        // For the dest, we use the offset
        destData = destStart + (width * y * 3) + (xOffset * 3);
        
        for (int x = xOffset; x < width; ++x)
        {
            unsigned char b = *blitData;
            unsigned char g = *(blitData + 1);
            unsigned char r = *(blitData + 2);

            if (!((b == B) && (g == G) && (r == R)))
            {
                // No clear color, copy from blit image
                *destData = b;
                *(destData + 1) = g;
                *(destData + 2) = r;
            }

            // Move one pixel along each row
            blitData += 3;
            destData += 3;
        }
    }
}
    
void Image::showImage(Image* image, std::string name)
{
    const char* windowName = 0;
    if (name.length() != 0)
        windowName = name.c_str();
    else
        windowName = DEBUG_WINDOW;

    cvNamedWindow(windowName, CV_WINDOW_AUTOSIZE);

    while(1)
    {
        cvShowImage(windowName, image->asIplImage());

        // Check for escape key
        char key;
        if ((key = (char)(cvWaitKey(10) & 255)) == 27)
            break;
    }

    cvDestroyWindow(windowName);
}
    
Image* Image::loadFromBuffer(unsigned char* buffer, int width, int height,
                             bool ownership)
{
    return new OpenCVImage(buffer, width, height, ownership);
}

} // namespace vision
} // namespace ram

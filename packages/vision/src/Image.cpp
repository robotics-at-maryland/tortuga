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
#include "vision/include/Image.h"

#include "math/include/Matrix3.h"

static const char* DEBUG_WINDOW = "Debug Image (close w/ESC Key)";

namespace ram {
namespace vision {

const size_t Image::formatDepthLookup[11] = {
    8, // PF_START
    8, // PF_RGB_8
    8, // PF_BGR_8
    8, // PF_YUV444_8
    8, // PF_GRAY_8
    8, // PF_HSV_8
    8, // PF_LUV_8
    8, // PF_LCHUV_8
    8, // PF_LAB_8
    8, // PF_LCHAB_8
    8  // PF_END
};

const size_t Image::formatChannelsLookup[11] = {
    3, // PF_START
    3, // PF_RGB_8
    3, // PF_BGR_8
    3, // PF_YUV444_8
    1, // PF_GRAY_8
    3, // PF_HSV_8
    3, // PF_LUV_8
    3, // PF_LCHUV_8
    3, // PF_LAB_8
    3, // PF_LCHAB_8
    3  // PF_END
};

Image* Image::loadFromFile(std::string fileName)
{
    IplImage* image = cvLoadImage(fileName.c_str());
    if (image)
	{
        return new OpenCVImage(image, true, Image::PF_BGR_8);
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

Image* Image::extractSubImage(Image* source, unsigned char* buffer,
                              int upperLeftX, int upperLeftY,
                              int lowerRightX, int lowerRightY)
{
    unsigned char* sourceBuffer = source->getData();
    PixelFormat fmt = source->getPixelFormat();
    unsigned char* srcPtr = sourceBuffer;
    unsigned char* destPtr = buffer;
    int width = lowerRightX - upperLeftX; //+ 1;
    int height = lowerRightY - upperLeftY; //+ 1;

    int yStart = upperLeftY;
    int yEnd = yStart + height;

    for (int y = yStart; y < yEnd; ++y)
    {
        // Get us to right row and column to start
        int offset = (y * source->asIplImage()->widthStep) + (upperLeftX * 3);
        srcPtr = sourceBuffer + offset;
        
        for (int x = 0; x < (width * 3); ++x)
        {
            *destPtr = *srcPtr;
            ++destPtr;
            ++srcPtr;
        }
    }

    return loadFromBuffer(buffer, width, height, false, fmt);
}

Image* Image::extractSubImage(Image* source,
                              int centerX, int centerY,
                              int width, int height,
                              unsigned char* buffer)
{
    int upperLeftX = centerX - (width - 1) / 2;
    int upperLeftY = centerY + (height - 1) / 2;
    int lowerRightX = centerX + (width + 1) / 2;
    int lowerRightY = centerY - (height + 1) / 2;

    return extractSubImage(source, buffer,
                           upperLeftX, upperLeftY,
                           lowerRightX, lowerRightY);
}




int Image::countWhitePixels(Image* source,
			    int upperLeftX, int upperLeftY,
			    int lowerRightX, int lowerRightY)
{
    unsigned char* sourceBuffer = source->getData();
    unsigned char* srcPtr = sourceBuffer;

    int width = lowerRightX - upperLeftX; //+ 1;
    int height = lowerRightY - upperLeftY; //+ 1;

    int yStart = upperLeftY;
    int yEnd = yStart + height;

    int whiteCount = 0;

    for (int y = yStart; y < yEnd; ++y)
    {
        // Get us to right row and column to start
        int offset = (y * source->asIplImage()->widthStep) + (upperLeftX * 3);
        srcPtr = sourceBuffer + offset;
        
        for (int x = 0; x < (width * 3); ++x)
        {
            // If white increment (note this is for subpixels)
            if (*srcPtr)
                whiteCount++;

            ++srcPtr;
        }
    }

    return whiteCount / 3;
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

void Image::fillMask(Image* src, Image* mask,
                     unsigned char R, unsigned char G, unsigned char B)
{
    assert(sameSize(src, mask) && "Images are not the same size");

    unsigned char *srcData = src->getData();
    unsigned char *maskData = mask->getData();
    
    int width = src->getWidth();
    int height = src->getHeight();
    int nChannels = src->getNumChannels();

    assert(nChannels == 3 && "only supports 3 channel images");

    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            if(*maskData)
            {
                *srcData = B;
                *(srcData + 1) = G;
                *(srcData + 2) = R;
            }
            srcData += 3;
            maskData += 3;
        }
    }
}

void Image::getAveragePixelValues(Image* source,
                                  int upperLeftX, int upperLeftY,
                                  int lowerRightX, int lowerRightY,
                                  double& channel1, double& channel2,
                                  double& channel3)
{
    unsigned char* sourceBuffer = source->getData();
    unsigned char* srcPtr = sourceBuffer;

    int width = lowerRightX - upperLeftX;
    int height = lowerRightY - upperLeftY;

    int yStart = upperLeftY;
    int yEnd = yStart + height;

    int channel1Total = 0;
    int channel2Total = 0;
    int channel3Total = 0;
    int pixelCount = width * height;

    assert(yStart < static_cast<int>(source->getHeight()) && "Too much height");
    assert(yEnd < static_cast<int>(source->getHeight()) && "Too much height");
    assert((upperLeftX + width) < static_cast<int>(source->getWidth()) 
           && "Too much width");
    
    for (int y = yStart; y < yEnd; ++y)
    {
        // Get us to right row and column to start
        int offset = (y * source->asIplImage()->widthStep) + (upperLeftX * 3);
        srcPtr = sourceBuffer + offset;
        
        for (int x = 0; x < width; x++)
        {
            // Accumulate the values for each pixel
            channel1Total += *srcPtr;
            channel2Total += *(srcPtr + 1);
            channel3Total += *(srcPtr + 2);
            srcPtr += 3;
        }
    }

    // Computer averages
    channel1 = static_cast<double>(channel1Total) / 
        static_cast<double>(pixelCount);
    channel2 = static_cast<double>(channel2Total) / 
        static_cast<double>(pixelCount);
    channel3 = static_cast<double>(channel3Total) / 
        static_cast<double>(pixelCount);
}

    
void Image::drawImage(Image* toWrite, int x, int y, Image* src, Image* dest)
{
    // Ensure to write is smaller
    assert((x + toWrite->getWidth()) <= src->getWidth() &&
           "ToWrite to wide");
    assert((y + toWrite->getHeight()) <= src->getHeight() &&
           "ToHeight to wide");

    // Copy image to dest if needed
    if (src != dest)
        cvCopy(src->asIplImage(), dest->asIplImage());

    size_t srcWidth = src->getWidth();
    size_t writeHeight = toWrite->getHeight();
    size_t writeWidth = toWrite->getWidth();
    unsigned char* destData = dest->getData();
    unsigned char* writeData = toWrite->getData();

    // Bring dest data into x,y poition
    destData += (x * 3) + (y * 3 * srcWidth);
    
    for (size_t i = 0; i < writeHeight; ++i)
    {
        memcpy(destData, writeData, writeWidth * 3);
        destData += (srcWidth * 3);
        writeData += (writeWidth * 3);
    }
}
    
void Image::writeText(Image* image, std::string text, int x, int y,
                      int height)
{
    int ymin = 0;
    // Use cvInitFont to create font as needed
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1);

    // Draw black backing square
    CvSize textSize;
    cvGetTextSize(text.c_str(), &font, &textSize, &ymin);
    CvPoint lowerRight = {x + textSize.width, y + textSize.height};
    CvPoint upperLeft = {x, y};
    
    cvRectangle(image->asIplImage(), upperLeft, lowerRight, CV_RGB(0,0,0),
                CV_FILLED);
    
    // Use cvPutText to draw text
    upperLeft.y += textSize.height;
    cvPutText(image->asIplImage(), text.c_str(), upperLeft, &font,
              CV_RGB(255,255,255));
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
                             bool ownership, PixelFormat fmt)
{
    return new OpenCVImage(buffer, width, height, ownership, fmt);
}

bool Image::sameSize(Image* imageA, Image* imageB)
{
  return ((imageA->getWidth() == imageB->getWidth()) && 
	  (imageB->getHeight() == imageB->getHeight()));
}

size_t Image::getFormatDepth(PixelFormat fmt)
{
    assert(fmt <= PF_END && "Invalid Pixel Format");
    assert(fmt >= PF_START && "Invalid Pixel Format");

    return formatDepthLookup[fmt];
}

size_t Image::getFormatNumChannels(PixelFormat fmt)
{
    assert(fmt <= PF_END && "Invalid Pixel Format");
    assert(fmt >= PF_START && "Invalid Pixel Format");

    return formatChannelsLookup[fmt];
}


} // namespace vision
} // namespace ram

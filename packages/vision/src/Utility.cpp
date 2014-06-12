/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/include/Utility.h
 */

// STD Includes
#include <cmath>

// Library Includes
#include "cv.h"

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/Utility.h"

namespace ram {
namespace vision {

int Utility::countWhitePixels(Image* source,
                              int upperLeftX, int upperLeftY,
                              int lowerRightX, int lowerRightY)
{
    unsigned char* sourceBuffer = source->getData();
    unsigned char* srcPtr = sourceBuffer;

    int width = lowerRightX - upperLeftX + 1;
    int height = lowerRightY - upperLeftY + 1;

    int nChannels = source->asIplImage()->nChannels;

    int yStart = upperLeftY;
    int yEnd = yStart + height;

    int whiteCount = 0;

    for (int y = yStart; y < yEnd; ++y)
    {
        // Get us to right row and column to start
        int offset = (y * source->asIplImage()->widthStep) + 
            (upperLeftX * nChannels);

        srcPtr = sourceBuffer + offset;
        
        for (int x = 0; x < (width * nChannels); ++x)
        {
            // If white increment (note this is for subpixels)
            if (*srcPtr)
                whiteCount++;

            ++srcPtr;
        }
    }

    return whiteCount / nChannels;
}

int Utility::countWhitePixels(Image* source, RegionOfInterest roi)
{
    return countWhitePixels(source, roi.minX(), roi.minY(),
                            roi.maxX(), roi.maxY());
}

double distanceToObjective(double actualWidth, double pixelWidth,
                           double fieldOfView, double imageWidth)
{
    double theta = fieldOfView * pixelWidth / imageWidth;
    double distance = actualWidth / (2 * std::tan(theta/2));

    return distance;
}



// void DC1394Camera::balanceWhite()
// {
//     // LOGGER.infoStream() << "Balancing U: " << m_vValue << " V: " << m_uValue
//     //                     << " Min: " << m_whiteMin
//     //                     << " Max: " << m_whiteMax;
    
//     // Gather RGB statistics
//     unsigned char* data = m_newFrame->image;
//     size_t numPixels = m_width * m_height;
//     double avg_r = 0, avg_g = 0, avg_b = 0;
    
//     for (size_t i = 0; i < numPixels; ++i)
//     {
//         avg_b += *data;
//         avg_g += *(data + 1);
//         avg_r += *(data + 2);
        
//         data += 3;
//     }

//     // Determine average values
//     avg_r /= numPixels;
//     avg_g /= numPixels;
//     avg_b /= numPixels;
//     // LOGGER.infoStream() << "Avg R: " << avg_r
//     //                     << " G: " << avg_g
//     //                     << " B: " << avg_b;

//     /// TODO: Make this tweakable?
//     double th = 5;
//     double vGain = 1;
//     double uGain = 1;

//     // Update white balance values
//     if((avg_r - avg_g) > th) {
//         // LOGGER.infoStream() << " Gain V: " << avg_g/avg_r * vGain;
//         m_vValue = m_vValue - 1;
//         //     m_vValue =  (unsigned int)((m_vValue*avg_g)/avg_r) * vGain;
//     } else if ((avg_g - avg_r) > th) {
//         // LOGGER.infoStream() << " Gain V: " << avg_r/avg_g * vGain;
//         m_vValue = m_vValue + 1;
//         //m_vValue =  (unsigned int)((m_vValue*avg_g)/avg_r) * vGain;
//     }
 
//     if((avg_b - avg_g) > th) {
//         // LOGGER.infoStream() << " Gain U: " << avg_g/avg_b * uGain;
//         m_uValue = m_uValue - 1;
//         //m_uValue =  (unsigned int)((m_uValue*avg_g)/avg_b) * uGain;
//     } else if ((avg_g - avg_b) > th) {
//         // LOGGER.infoStream() << " Gain U: " << avg_b/avg_g * uGain;
//         m_uValue = m_uValue + 1;
//         // m_uValue =  (unsigned int)((m_uValue*avg_g)/avg_b) * uGain;
//     }

//     // Handle zero based white balance
//     if (m_vValue < 1)
//         m_vValue = (m_whiteMax + m_whiteMin) / 2;
//     if (m_uValue < 1)
//         m_uValue = (m_whiteMax + m_whiteMin) / 2;

//     // LOGGER.infoStream() << "Balance U: " << m_vValue << " V: " << m_uValue;
    
//     // Clamp with min and maximum values
//     if (m_vValue > m_whiteMax)
//         m_vValue = m_whiteMax;
//     else if (m_vValue < m_whiteMin)
//         m_vValue = m_whiteMin;

//     if (m_uValue > m_whiteMax)
//         m_uValue = m_whiteMax;
//     else if (m_uValue < m_whiteMin)
//         m_uValue = m_whiteMin;
    
//     // Set the new values
//     dc1394error_t err =
//         dc1394_feature_whitebalance_set_value(m_camera, m_uValue, m_vValue);
//     assert(DC1394_SUCCESS == err && "Could not set whitebalance");

//     // LOGGER.infoStream() << "Done Balance U: " << m_vValue
//     //                     << " V: " << m_uValue;
// }
    

} // namespace vision
} // namespace ram

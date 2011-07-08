/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/FANNLetterDetector.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <cv.h>

// Project Includes
#include "vision/include/FANNLetterDetector.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

#define SYMBOL_LARGE_X 0x01
#define SYMBOL_SMALL_X 0x02
#define SYMBOL_LARGE_O 0x04
#define SYMBOL_SMALL_O 0x08
#define SYMBOL_COUNT 4

#define FEATURE_COUNT 10

namespace ram {
namespace vision {

FANNLetterDetector::FANNLetterDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    FANNSymbolDetector(FEATURE_COUNT, SYMBOL_COUNT, config, eventHub),
    m_resizedImage(new OpenCVImage(640, 480, Image::PF_BGR_8)),
    m_blobDetector()
{
}

void FANNLetterDetector::processImage(Image* input, Image* output)
{
    runNN(input);
}

Symbol::SymbolType FANNLetterDetector::getSymbol()
{
    switch (getResult())
    {
        case SYMBOL_LARGE_X:
            return Symbol::LARGE_X;
        case SYMBOL_SMALL_X:
            return Symbol::SMALL_X;
        case SYMBOL_LARGE_O:
            return Symbol::LARGE_O;
        case SYMBOL_SMALL_O:
            return Symbol::SMALL_O;
        default:
            return Symbol::UNKNOWN;
    }
}

void FANNLetterDetector::getImageFeatures(Image* inputImage, float *features)
{
    m_resizedImage->copyFrom(inputImage);
    m_resizedImage->setPixelFormat(Image::PF_GRAY_8);
    m_blobDetector.processImage(m_resizedImage);

    std::vector<BlobDetector::Blob> blobs = m_blobDetector.getBlobs();
    BlobDetector::Blob symbolBlob;
    
    // get the largest blob found
    if(blobs.size() > 0)
        symbolBlob = blobs[0];

    // True if the image is wider then it is tall
    bool wide = true;
    double width = m_resizedImage->getWidth();
    double height = m_resizedImage->getHeight();
    double trueAspectRatio = width / height;
    float aspectRatio = trueAspectRatio;
    if (aspectRatio < 1)
    {
        wide = false;
        aspectRatio = 1.0 / aspectRatio;
    }

    double widthPct = symbolBlob.getWidth() / width;
    double heightPct = symbolBlob.getHeight() / height;
    CvMoments moments;
    cvMoments(m_resizedImage->asIplImage(), &moments, true);
    CvHuMoments huMoments;
    cvGetHuMoments(&moments, &huMoments);

    int featureNum = 0;

    features[featureNum++] = widthPct;
    features[featureNum++] = heightPct;
    features[featureNum++] = huMoments.hu6;

    // Determine amount of red on each "long" half of the image and return the
    // ratio of the two together
    double average1, average2, dummy2, dummy3;
    if (wide)
    {
        // Wider then we are tall (ie. the image is wide)

        // Get the top half
        Image::getAveragePixelValues(m_resizedImage, 0, 0, width - 1, height/2 - 1,
                                     average1, dummy2, dummy3);

        // Get the bottom half
        Image::getAveragePixelValues(m_resizedImage, 0, height/2,
                                     width - 1, height - 1,
                                     average2, dummy2, dummy3);
    }
    else
    {
        // Taller then we are wide (ie. the image is skinny)

        // Get the left side
        Image::getAveragePixelValues(m_resizedImage, 0, 0,
                                     width/2 - 1, height - 1,
                                     average1, dummy2, dummy3);

        // Get the right side
        Image::getAveragePixelValues(m_resizedImage, width/2, 0,
                                     width - 1, height- 1,
                                     average2, dummy2, dummy3);
    }
    
    // Make sure the ratio is always positive
    double ratio = 0;
    if (average2 != 0)
        ratio = average1/average2;
    if (ratio < 1)
        ratio = 1.0 / ratio;
    features[featureNum++] = (float)ratio;
    
    // Determine the ammount of red in all the corners of the image

    // Corner size
    int cornerSize = 0;
    if (wide)
        cornerSize = height / 8;
    else
        cornerSize = width / 8;

    // If everything is too small 
    if (cornerSize == 0)
        cornerSize = 1;

    // Find the average in all the corners
    double average3, average4;

    // Upper Left
    Image::getAveragePixelValues(m_resizedImage, 0, 0,
                                 cornerSize, cornerSize,
                                 average1, dummy2, dummy3);
    
    // Upper right
    Image::getAveragePixelValues(m_resizedImage, width - cornerSize, 0,
                                 width - 1, cornerSize,
                                 average2, dummy2, dummy3);

    // Lower Left
    Image::getAveragePixelValues(m_resizedImage, 0, height - cornerSize,
                                 cornerSize, height - 1,
                                 average3, dummy2, dummy3);

    // Lower Right
    Image::getAveragePixelValues(m_resizedImage, width - cornerSize,
                                 height - cornerSize,
                                 width - 1, height - 1,
                                 average4, dummy2, dummy3);

    double averageCorner = (average1 + average2 + average3 + average4) / 4;
    features[featureNum++] = (float)averageCorner / 255.0f;

    // Find the fill percent of the central region
    if (wide)
    {
        // Wider then we are tall (ie. the image is wide)
        int middle = width/2;

        Image::getAveragePixelValues(m_resizedImage,
                                     middle - cornerSize, 0,
                                     middle + cornerSize, height - 1,
                                     average1, dummy2, dummy3);
    }
    else
    {
        // Taller then we are wide (ie. the image is skinny)
        int middle = height/2;

        Image::getAveragePixelValues(m_resizedImage,
                                     0, middle - cornerSize,
                                     width - 1, middle + cornerSize,
                                     average1, dummy2, dummy3);
    }

    features[featureNum++] = static_cast<float>(average1 / 255.0f);
}

} // namespace vision
} // namespace ram

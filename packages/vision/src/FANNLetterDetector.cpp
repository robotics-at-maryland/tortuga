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
#include <cmath>
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
#define FEATURE_COUNT 4

namespace ram {
namespace vision {

FANNLetterDetector::FANNLetterDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    FANNSymbolDetector(FEATURE_COUNT, SYMBOL_COUNT, config, eventHub),
    m_frame(new OpenCVImage(640, 480, Image::PF_BGR_8)),
    m_croppedFrame(new OpenCVImage(640, 480, Image::PF_BGR_8)),
    m_scratchBuffer(new unsigned char[640 * 480 * 3]),
    m_blobDetector()
{
}

FANNLetterDetector::~FANNLetterDetector()
{
    delete m_frame;
    delete m_croppedFrame;
    delete[] m_scratchBuffer;
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
    m_frame->copyFrom(inputImage);
    m_frame->setPixelFormat(Image::PF_BGR_8);
    double imgWidth = m_frame->getWidth();
    double imgHeight = m_frame->getHeight();
    // look for a big blob inside the frame
    m_blobDetector.setMinimumBlobSize(100);
    m_blobDetector.processImage(m_frame);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();
    BlobDetector::Blob symbolBlob;

    // get the largest blob found
    // if we see a crappy blob its not a problem because the features 
    // will just be garbage and be ignored
    if(m_blobDetector.found())
        symbolBlob = blobs.front();
    else // otherwise we cant really do anything if we dont see a blob
        return;

    Image *processingFrame;
    processingFrame = cropBinImage(m_frame, m_scratchBuffer);
    if(!processingFrame)
        assert(false && "error croping the image");
    m_croppedFrame->copyFrom(processingFrame);

    delete processingFrame;

    // get some information from the blob to help us compute features later
    double symbolWidth = symbolBlob.getWidth();
    double symbolHeight = symbolBlob.getHeight();
    // double symbolMinX = symbolBlob.getMinX();
    // double symbolMaxX = symbolBlob.getMaxX();
    // double symbolMinY = symbolBlob.getMinY();
    // double symbolMaxY = symbolBlob.getMaxY();

    // True if the image is wider then it is tall
    bool wide = true;
    double trueAspectRatio = symbolWidth / symbolHeight;
    // because the image may be rotated, we choose to make the aspect 
    // ratio greater than 1
    float aspectRatio = trueAspectRatio;
    if (aspectRatio < 1)
    {
        wide = false;
        aspectRatio = 1.0 / aspectRatio;
    }

    double relativeSymbolWidth = symbolWidth / imgWidth;
    double relativeSymbolHeight = symbolHeight / imgHeight;
    
    // pixel percentage of symbol relative to the entire image of the inside of the bin
    double pixelPercentage = symbolBlob.getSize() / (imgWidth * imgHeight);

    // these will all end up as the same value since this is a 'binary' image
    double pixelAverageCh1, pixelAverageCh2, pixelAverageCh3;
    int hstep = m_croppedFrame->getWidth() / 5;
    int vstep = m_croppedFrame->getHeight() / 5;
    int upperLeftX = 2 * hstep;
    int upperLeftY = 2 * vstep;
    int lowerRightX = m_croppedFrame->getWidth() - 2 * hstep;
    int lowerRightY = m_croppedFrame->getHeight() - 2 * vstep;

    Image::getAveragePixelValues(m_croppedFrame, upperLeftX, upperLeftY,
                                 lowerRightX, lowerRightY, pixelAverageCh1,
                                 pixelAverageCh2, pixelAverageCh3);

    m_croppedFrame->setPixelFormat(Image::PF_GRAY_8);
    CvMoments moments;
    cvMoments(m_croppedFrame->asIplImage(), &moments, true);
    CvHuMoments huMoments;
    cvGetHuMoments(&moments, &huMoments);

    features[0] = relativeSymbolWidth;
    features[1] = relativeSymbolHeight;
    features[2] = pixelPercentage;
    features[3] = pixelAverageCh1;
}

Image* FANNLetterDetector::cropBinImage(Image* binImage, unsigned char *buffer)
{   
    m_blobDetector.setMinimumBlobSize(100);
    m_blobDetector.processImage(binImage);
    if (!m_blobDetector.found())
        return 0;
        
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    // blobs are ordered largest to smallest so take the largest
    BlobDetector::Blob biggestBlob = blobs[0];
    int upperLeftX = biggestBlob.getMinX();
    int upperLeftY = biggestBlob.getMinY();
    int lowerRightX = biggestBlob.getMaxX();
    int lowerRightY = biggestBlob.getMaxY();
    
    Image* croppedImage = Image::extractSubImage(
        binImage, buffer, upperLeftX, upperLeftY,
        lowerRightX, lowerRightY);

    return croppedImage;
}

} // namespace vision
} // namespace ram

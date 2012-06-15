/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/FANNGladiatorDetector.cpp
 */

// STD Includes
#include <iostream>
#include <cmath>
// Library Includes
#include <cv.h>

// Project Includes
#include "vision/include/FANNGladiatorDetector.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

#define GLADIATOR_SYMBOL_NET 0x01
#define GLADIATOR_SYMBOL_SHIELD 0x02
#define GLADIATOR_SYMBOL_SWORD 0x04
#define GLADIATOR_SYMBOL_TRIDENT 0x08
#define GLADIATOR_SYMBOL_COUNT 4
#define GLADIATOR_FEATURE_COUNT 6

namespace ram {
namespace vision {

FANNGladiatorDetector::FANNGladiatorDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    FANNSymbolDetector(GLADIATOR_FEATURE_COUNT, GLADIATOR_SYMBOL_COUNT, config, eventHub),
    m_frame(new OpenCVImage(640, 480, Image::PF_BGR_8)),
    m_croppedFrame(new OpenCVImage(640, 480, Image::PF_BGR_8)),
    m_scratchBuffer(new unsigned char[640 * 480 * 3]),
    m_blobDetector()
{
}

FANNGladiatorDetector::~FANNGladiatorDetector()
{
    delete m_frame;
    delete m_croppedFrame;
    delete[] m_scratchBuffer;
}

void FANNGladiatorDetector::processImage(Image* input, Image* output)
{
    runNN(input);
}

Symbol::SymbolType FANNGladiatorDetector::getSymbol()
{
    switch (getResult())
    {
        case GLADIATOR_SYMBOL_NET:
            return Symbol::NET;
        case GLADIATOR_SYMBOL_SHIELD:
            return Symbol::SHIELD;
        case GLADIATOR_SYMBOL_SWORD:
            return Symbol::SWORD;
        case GLADIATOR_SYMBOL_TRIDENT:
            return Symbol::TRIDENT;
        default:
            return Symbol::UNKNOWN;
    }
}

void FANNGladiatorDetector::getImageFeatures(Image* inputImage, float *features)
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

    double centerAvg = pixelAverageCh1;

    int croppedWidth = m_croppedFrame->getWidth();
    int croppedHeight = m_croppedFrame->getHeight();

    Image::getAveragePixelValues(m_croppedFrame, 0, 0, croppedWidth/2, 
                                 croppedHeight/2, pixelAverageCh1,
                                 pixelAverageCh2, pixelAverageCh3);

    double upperDiagAvg = pixelAverageCh1 / 2;

    Image::getAveragePixelValues(m_croppedFrame, croppedWidth/2, 
                                 croppedHeight/2, croppedWidth,
                                 croppedHeight, pixelAverageCh1,
                                 pixelAverageCh2, pixelAverageCh3);

    upperDiagAvg += pixelAverageCh1 / 2;

    Image::getAveragePixelValues(m_croppedFrame, 0, croppedHeight/2, 
                                 croppedWidth/2, 
                                 croppedHeight, pixelAverageCh1,
                                 pixelAverageCh2, pixelAverageCh3);

    double lowerDiagAvg = pixelAverageCh1 / 2;

    Image::getAveragePixelValues(m_croppedFrame, croppedWidth/2, 
                                 0, croppedWidth,
                                 croppedHeight/2, pixelAverageCh1,
                                 pixelAverageCh2, pixelAverageCh3);

    lowerDiagAvg += pixelAverageCh1 / 2;


    m_croppedFrame->setPixelFormat(Image::PF_GRAY_8);
    CvMoments moments;
    cvMoments(m_croppedFrame->asIplImage(), &moments, true);
    CvHuMoments huMoments;
    cvGetHuMoments(&moments, &huMoments);

    features[0] = relativeSymbolWidth;
    features[1] = relativeSymbolHeight;
    features[2] = pixelPercentage;
    features[3] = centerAvg;

    features[4] = upperDiagAvg;
    features[5] = lowerDiagAvg;
}

Image* FANNGladiatorDetector::cropBinImage(Image* binImage, unsigned char *buffer)
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

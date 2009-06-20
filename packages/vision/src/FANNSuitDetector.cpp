/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/src/FANNSuiteDetector.cpp
 */

#include <opencv/cv.h>

#include "vision/include/OpenCVImage.h"

// header for this class
#include "vision/include/FANNSuitDetector.hpp"

// some unfortuanate constants
//  - these depend on the loaded network file for accuracy
#define SUIT_CLUB 0
#define SUIT_DIAMOND 1
#define SUIT_HEART 2
#define SUIT_SPADE 3

namespace ram {
namespace vision {
        
FANNSuitDetector::FANNSuitDetector(core::ConfigNode config,
                                   core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_imageDetector(ImageDetector(config, eventHub)),
    m_analyzed(0)
{
}
    
void FANNSuitDetector::processImage(Image* input, Image* output)
{
    IplImage* edgeImage = cvCreateImage (cvSize (input->getWidth(), input->getHeight()), IPL_DEPTH_8U, 1);
    IplImage* newIn = ImageIdentifier::grayscale (input->asIplImage());
    Image* edges = new OpenCVImage (edgeImage);
    cvCanny (newIn, edges->asIplImage(), 25, 200, 3);
    m_analyzed = edges->asIplImage();
    m_imageDetector.processImage(edges);
    if (output) {
        output->copyFrom (edges);
    }
}
    
IplImage* FANNSuitDetector::getAnalyzedImage()
{
    return m_analyzed;
}
    
Symbol::SymbolType FANNSuitDetector::getSuit()
{
    switch (m_imageDetector.getResult())
    {
        case SUIT_CLUB:
            return Symbol::CLUB;
            break;
        case SUIT_DIAMOND:
            return Symbol::DIAMOND;
            break;
        case SUIT_HEART:
            return Symbol::HEART;
            break;
        case SUIT_SPADE:
            return Symbol::SPADE;
            break;
        default:
            return Symbol::UNKNOWN;
            break;
    };
}
    
} // namespace vision
} // namespace ram

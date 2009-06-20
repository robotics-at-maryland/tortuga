/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/src/FANNSymboleDetector.cpp
 */

#include <opencv/cv.h>

#include "vision/include/OpenCVImage.h"

// header for this class
#include "vision/include/FANNSymbolDetector.hpp"

// some unfortuanate constants
//  - these depend on the loaded network file for accuracy
#define SYMBOL_CLUB 0
#define SYMBOL_DIAMOND 1
#define SYMBOL_HEART 2
#define SYMBOL_SPADE 3

namespace ram {
namespace vision {
        
FANNSymbolDetector::FANNSymbolDetector(core::ConfigNode config,
                                   core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_imageDetector(ImageDetector(config, eventHub)),
    m_analyzed(0)
{
}
    
void FANNSymbolDetector::processImage(Image* input, Image* output)
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
    
IplImage* FANNSymbolDetector::getAnalyzedImage()
{
    return m_analyzed;
}
    
Symbol::SymbolType FANNSymbolDetector::getSymbol()
{
    switch (m_imageDetector.getResult())
    {
        case SYMBOL_CLUB:
            return Symbol::CLUB;
            break;
        case SYMBOL_DIAMOND:
            return Symbol::DIAMOND;
            break;
        case SYMBOL_HEART:
            return Symbol::HEART;
            break;
        case SYMBOL_SPADE:
            return Symbol::SPADE;
            break;
        default:
            return Symbol::UNKNOWN;
            break;
    };
}
    
} // namespace vision
} // namespace ram

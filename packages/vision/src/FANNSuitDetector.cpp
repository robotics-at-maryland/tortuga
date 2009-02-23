/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/src/FANNSuiteDetector.cpp
 */

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
    if (output) {
        //(*output) = (*input);
    }
    m_analyzed = input->asIplImage();
    m_imageDetector.processImage(input);
}
    
IplImage* FANNSuitDetector::getAnalyzedImage()
{
    return m_analyzed;
}
    
Suit::SuitType FANNSuitDetector::getSuit()
{
    switch (m_imageDetector.getResult())
    {
        case SUIT_CLUB:
            return Suit::CLUB;
            break;
        case SUIT_DIAMOND:
            return Suit::DIAMOND;
            break;
        case SUIT_HEART:
            return Suit::HEART;
            break;
        case SUIT_SPADE:
            return Suit::SPADE;
            break;
        default:
            return Suit::UNKNOWN;
            break;
    };
}
    
} // namespace vision
} // namespace ram

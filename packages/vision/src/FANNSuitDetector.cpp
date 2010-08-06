/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/src/FANNSuitDetector.cpp
 */

// Library Includes
#include <opencv/cv.h>

// Project Includes
#include "vision/include/FANNSuitDetector.h"
#include "vision/include/OpenCVImage.h"


// some unfortuanate constants
//  - these depend on the loaded network file for accuracy
#define SYMBOL_CLUB 0
#define SYMBOL_DIAMOND 1
#define SYMBOL_HEART 2
#define SYMBOL_SPADE 3
#define SYMBOL_COUNT 4

#define IMAGE_WIDTH 64
#define IMAGE_HEIGHT 64
#define FEATURE_COUNT ((IMAGE_WIDTH) * (IMAGE_HEIGHT))

namespace ram {
namespace vision {
        
FANNSuitDetector::FANNSuitDetector(core::ConfigNode config,
                                   core::EventHubPtr eventHub) :
    FANNSymbolDetector(FEATURE_COUNT, SYMBOL_COUNT, config, eventHub),
    m_resizedImage(new OpenCVImage(IMAGE_WIDTH, IMAGE_HEIGHT))
{
}
    
void FANNSuitDetector::processImage(Image* input, Image* output)
{
    runNN(input);
}

void FANNSuitDetector::getImageFeatures(Image* inputImage, float* features)
{
    // Resize the image
    cvResize(inputImage->asIplImage(), m_resizedImage->asIplImage(),
             CV_INTER_LINEAR);

    // TODO any post processing
    
    // ASSUME our classic "gray scale" images (ie. black and white RGB)
    unsigned int size = inputImage->getWidth() * inputImage->getHeight() * 3;
    unsigned char* data = inputImage->getData();

    for (unsigned int i = 0; i < size; i += 3)
    {
        *features = (float)*data;
        features += 1;
        data += 3;
    }
}
    
Symbol::SymbolType FANNSuitDetector::getSymbol()
{
    switch (getResult())
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

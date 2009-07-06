/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/BasicWW2Detector.cpp
 */

// Project Includes
#include "vision/include/FANNWW2Detector.h"
#include "vision/include/Image.h"


// some unfortuanate constants
//  - these depend on the loaded network file for accuracy
#define SYMBOL_SHIP 0
#define SYMBOL_AIRCRAFT 1
#define SYMBOL_TANK 2
#define SYMBOL_FACTORY 3
#define SYMBOL_COUNT 4

#define FEATURE_COUNT 3


namespace ram {
namespace vision {
        
FANNWW2Detector::FANNWW2Detector(core::ConfigNode config,
                                   core::EventHubPtr eventHub) :
    FANNSymbolDetector(FEATURE_COUNT, SYMBOL_COUNT, config, eventHub)
{
}
    
void FANNWW2Detector::processImage(Image* input, Image* output)
{
    runNN(input);
}

void FANNWW2Detector::getImageFeatures(Image* inputImage, float* features)
{
    // True if the image is wider then it is tall
    bool wide = true;
    int width = inputImage->getWidth();
    int height = inputImage->getHeight();

    // Determine aspect ratio (ensure the ratio is always > 1)
    double trueAspectRatio = ((double)width) / ((double)height);
    double aspectRatio = trueAspectRatio;
    if (aspectRatio < 1)
    {
        wide = false;
        aspectRatio = 1.0 / aspectRatio;
    }
    features[0] = (float)aspectRatio;

    // Determine amount of red on each "long" half of the image and return the
    // ratio of the two together
    double average1, average2, dummy2, dummy3;
    if (wide)
    {
        // Wider then we are tall (ie. the image is wide)

        // Get the top half
        Image::getAveragePixelValues(inputImage, 0, 0, width - 1, height/2 - 1,
                                     average1, dummy2, dummy3);

        // Get the bottom half
        Image::getAveragePixelValues(inputImage, 0, height/2,
                                     width - 1, height - 1,
                                     average2, dummy2, dummy3);
    }
    else
    {
        // Taller then we are wide (ie. the image is skinny)

        // Get the left side
        Image::getAveragePixelValues(inputImage, 0, 0,
                                     width/2 - 1, height - 1,
                                     average1, dummy2, dummy3);

        // Get the right side
        Image::getAveragePixelValues(inputImage, width/2, 0,
                                     width - 1, height- 1,
                                     average2, dummy2, dummy3);
    }
    
    // Make sure the ratio is always positive
    double ratio = 0;
    if (average2 != 0)
        ratio = average1/average2;
    if (ratio < 1)
        ratio = 1.0 / ratio;
    features[1] = (float)ratio;
    
    // Determine the ammount of red in all the corners of the image

    // Corner size
    int cornerSize = 0;
    if (wide)
        cornerSize = height / 8;
    else
        cornerSize = width / 8;

    // Find the average in all the corners
    double average3, average4;

    // Upper Left
    Image::getAveragePixelValues(inputImage, 0, 0,
                                 cornerSize, cornerSize,
                                 average1, dummy2, dummy3);
    
    // Upper right
    Image::getAveragePixelValues(inputImage, width - cornerSize, 0,
                                 width - 1, cornerSize,
                                 average2, dummy2, dummy3);

    // Lower Left
    Image::getAveragePixelValues(inputImage, 0, height - cornerSize,
                                 cornerSize, height - 1,
                                 average3, dummy2, dummy3);

    // Lower Right
    Image::getAveragePixelValues(inputImage, width - cornerSize,
                                 height - cornerSize,
                                 width - 1, height - 1,
                                 average4, dummy2, dummy3);

    double averageCorner = (average1 + average2 + average3 + average4) / 4;
    features[2] = (float)averageCorner / 255.0f;
}
    
Symbol::SymbolType FANNWW2Detector::getSymbol()
{
    switch (getResult())
    {
        case SYMBOL_SHIP:
            return Symbol::SHIP;
            break;
        case SYMBOL_AIRCRAFT:
            return Symbol::AIRCRAFT;
            break;
        case SYMBOL_TANK:
            return Symbol::TANK;
            break;
        case SYMBOL_FACTORY:
            return Symbol::FACTORY;
            break;
        default:
            return Symbol::UNKNOWN;
            break;
    };
}
    
} // namespace vision
} // namespace ram

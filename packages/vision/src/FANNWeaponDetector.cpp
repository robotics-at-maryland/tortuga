/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/FANNWeaponDetector.cpp
 */

// Project Includes
#include "vision/include/FANNWeaponDetector.h"
#include "vision/include/Image.h"

#define SYMBOL_AXE 0
#define SYMBOL_CLIPPERS 1
#define SYMBOL_HAMMER 2
#define SYMBOL_MACHETE 3
#define SYMBOL_COUNT 4

#define FEATURE_COUNT 4

namespace ram {
namespace vision {

FANNWeaponDetector::FANNWeaponDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    FANNSymbolDetector(FEATURE_COUNT, SYMBOL_COUNT, config, eventHub)
{
}

void FANNWeaponDetector::processImage(Image* input, Image* output)
{
    runNN(input);
}

Symbol::SymbolType FANNWeaponDetector::getSymbol()
{
    switch (getResult())
    {
        case SYMBOL_AXE:
            return Symbol::AXE;
        case SYMBOL_CLIPPERS:
            return Symbol::CLIPPERS;
        case SYMBOL_HAMMER:
            return Symbol::HAMMER;
        case SYMBOL_MACHETE:
            return Symbol::MACHETE;
        default:
            return Symbol::UNKNOWN;
    }
}

void FANNWeaponDetector::getImageFeatures(Image* inputImage, float *features)
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

    // If everything is too small 
    if (cornerSize == 0)
        cornerSize = 1;

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

    // Find the fill percent of the central region
    if (wide)
    {
        // Wider then we are tall (ie. the image is wide)
        int middle = width/2;

        Image::getAveragePixelValues(inputImage,
                                     middle - cornerSize, 0,
                                     middle + cornerSize, height - 1,
                                     average1, dummy2, dummy3);
    }
    else
    {
        // Taller then we are wide (ie. the image is skinny)
        int middle = height/2;

        Image::getAveragePixelValues(inputImage,
                                     0, middle - cornerSize,
                                     width - 1, middle + cornerSize,
                                     average1, dummy2, dummy3);
    }

    features[3] = (float)average1 / 255.0f;
}

} // namespace vision
} // namespace ram

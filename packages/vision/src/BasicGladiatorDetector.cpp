/*
 * Copyright (C) 2012 Robotics at Maryland
 * Copyright (C) 2012 Gary Sullivan
 * All rights reserved.
 *
 * Author: Gary Sullivan <gsulliva@umd.edu>
 * File:  packages/vision/src/BasicGladiatorDetector.cpp
 */

// STD Includes
#include <iostream>

// Project Includes
#include "vision/include/BasicGladiatorDetector.h"
#include "vision/include/FANNGladiatorDetector.h"
#include "vision/include/Symbol.h"
#include "core/include/PropertySet.h"
#include "vision/include/Image.h"

namespace ram {
namespace vision {


double BasicGladiatorDetector::RELATIVE_SYMBOL_WIDTH_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT] =
{0.615, 0.621, 0.626, 0.441};
double BasicGladiatorDetector::RELATIVE_SYMBOL_WIDTH_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{0.049, 0.033, 0.035, 0.099};

double BasicGladiatorDetector::RELATIVE_SYMBOL_HEIGHT_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{0.598, 0.579, 0.665, 0.581};
double BasicGladiatorDetector::RELATIVE_SYMBOL_HEIGHT_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{0.054, 0.015, 0.039, 0.117};

double BasicGladiatorDetector::PIXEL_PERCENTAGE_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{0.235, 0.293, 0.0831, 0.0515};
double BasicGladiatorDetector::PIXEL_PERCENTAGE_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{0.054, 0.025, 0.0077, 0.0124};

double BasicGladiatorDetector::CENTER_PIXEL_PERCENTAGE_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT] =
{211, 212, 199, 90};
double BasicGladiatorDetector::CENTER_PIXEL_PERCENTAGE_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT] =
{30, 12, 10, 40};

double BasicGladiatorDetector::UPPER_DIAGONAL_PIXEL_PERCENTAGE_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{176, 228, 98, 35};
double BasicGladiatorDetector::UPPER_DIAGONAL_PIXEL_PERCENTAGE_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{23, 5, 6, 19};

double BasicGladiatorDetector::LOWER_DIAGONAL_PIXEL_PERCENTAGE_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{164, 208, 8, 76};
double BasicGladiatorDetector::LOWER_DIAGONAL_PIXEL_PERCENTAGE_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT] = 
{27, 17, 2, 11};

BasicGladiatorDetector::BasicGladiatorDetector(core::ConfigNode config,
                                         core::EventHubPtr eventHub) :
    SymbolDetector(eventHub)
{
    m_fannDetector = FANNSymbolDetectorPtr(
        new FANNGladiatorDetector(core::ConfigNode::fromString("{'training':1}")));
    assert(GLADIATOR_BASIC_FEATURE_COUNT == m_fannDetector->getNumberFeatures());


    addProps("RelativeWidth", m_relativeSymbolWidthMean, m_relativeSymbolWidthStDev,
             RELATIVE_SYMBOL_WIDTH_MEAN, RELATIVE_SYMBOL_WIDTH_STDEV, config);

    addProps("RelativeHeight", m_relativeSymbolHeightMean, m_relativeSymbolHeightStDev,
             RELATIVE_SYMBOL_HEIGHT_MEAN, RELATIVE_SYMBOL_HEIGHT_STDEV, config);

    addProps("PixelPercentage", m_pixelPercentageMean, m_pixelPercentageStDev,
             PIXEL_PERCENTAGE_MEAN, PIXEL_PERCENTAGE_STDEV, config);

    addProps("CenterPixelPercentage", m_centerPixelPercentageMean,
             m_centerPixelPercentageStDev, CENTER_PIXEL_PERCENTAGE_MEAN,
             CENTER_PIXEL_PERCENTAGE_STDEV, config);

    addProps("UpperDiagonalPixelPercentage", m_upperDiagonalPixelPercentageMean,
             m_upperDiagonalPixelPercentageStDev, 
             UPPER_DIAGONAL_PIXEL_PERCENTAGE_MEAN,
             UPPER_DIAGONAL_PIXEL_PERCENTAGE_STDEV, config);

    addProps("LowerDiagonalPixelPercentage", m_lowerDiagonalPixelPercentageMean,
             m_lowerDiagonalPixelPercentageStDev, 
             LOWER_DIAGONAL_PIXEL_PERCENTAGE_MEAN,
             LOWER_DIAGONAL_PIXEL_PERCENTAGE_STDEV, config);
}

void BasicGladiatorDetector::processImage(Image *input, Image *output)
{
    // Assume we don't know what it is
    m_symbol = Symbol::NONEFOUND;
    
    // Get the features from the image
    float features[GLADIATOR_BASIC_FEATURE_COUNT];
    m_fannDetector->getImageFeatures(input, features);

    double symbolLikelihood[GLADIATOR_BASIC_SYMBOL_COUNT] = {1};
    getLikelihoodOfSymbol(features, symbolLikelihood);

    // std::cout << "P(X) = " << symbolLikelihood[0] << std::endl;
    // std::cout << "P(x) = " << symbolLikelihood[1] << std::endl;
    // std::cout << "P(O) = " << symbolLikelihood[2] << std::endl;
    // std::cout << "P(o) = " << symbolLikelihood[3] << std::endl;

    double maxLikelihood = symbolLikelihood[0];
    m_symbol = Symbol::NET;

    if(symbolLikelihood[1] > maxLikelihood)
    {
        maxLikelihood = symbolLikelihood[1];
        m_symbol = Symbol::SHIELD;
    }

    if(symbolLikelihood[2] > maxLikelihood)
    {
        maxLikelihood = symbolLikelihood[2];
        m_symbol = Symbol::SWORD;
    }

    if(symbolLikelihood[3] > maxLikelihood)
    {
        maxLikelihood = symbolLikelihood[3];
        m_symbol = Symbol::TRIDENT;
    }
    
    if(maxLikelihood < 0.01)
        m_symbol = Symbol::UNKNOWN;
}

void BasicGladiatorDetector::getLikelihoodOfSymbol(float* features, double* resultLikelihood)
{
    double likelihood[GLADIATOR_BASIC_SYMBOL_COUNT][GLADIATOR_BASIC_FEATURE_COUNT] = {{0}};

    for(int symIdx = 0; symIdx < GLADIATOR_BASIC_SYMBOL_COUNT; symIdx++)
    {
        likelihood[symIdx][0] = gaussian1DLikelihood(features[0],
                                                     m_relativeSymbolWidthMean[symIdx],
                                                     m_relativeSymbolWidthStDev[symIdx]);

        likelihood[symIdx][1] = gaussian1DLikelihood(features[1], 
                                                     m_relativeSymbolHeightMean[symIdx],
                                                     m_relativeSymbolHeightStDev[symIdx]);

        likelihood[symIdx][2] = gaussian1DLikelihood(features[2], 
                                                     m_pixelPercentageMean[symIdx],
                                                     m_pixelPercentageStDev[symIdx]);

        likelihood[symIdx][3] = gaussian1DLikelihood(features[3], 
                                                     m_centerPixelPercentageMean[symIdx],
                                                     m_centerPixelPercentageStDev[symIdx]);
        
        likelihood[symIdx][4] = gaussian1DLikelihood(features[4], 
                                                     m_upperDiagonalPixelPercentageMean[symIdx],
                                                     m_upperDiagonalPixelPercentageStDev[symIdx]);
        likelihood[symIdx][5] = gaussian1DLikelihood(features[5], 
                                                     m_lowerDiagonalPixelPercentageMean[symIdx],
                                                     m_lowerDiagonalPixelPercentageStDev[symIdx]);
    }

    double totalFeatureLikelihood[GLADIATOR_BASIC_FEATURE_COUNT] = {0};
    
    for(int f = 0; f < GLADIATOR_BASIC_FEATURE_COUNT; f++)
    {
        for(int s = 0; s < GLADIATOR_BASIC_SYMBOL_COUNT; s++)
        {
            totalFeatureLikelihood[f] += likelihood[s][f];
        }
    }

    for(int s = 0; s < GLADIATOR_BASIC_SYMBOL_COUNT; s++)
    {
        resultLikelihood[s] = 1;
    }
    
    for(int f = 0; f < GLADIATOR_BASIC_FEATURE_COUNT; f++)
    {
        for(int s = 0; s < GLADIATOR_BASIC_SYMBOL_COUNT; s++)
        {
            likelihood[s][f] /= totalFeatureLikelihood[f];
            resultLikelihood[s] *= likelihood[s][f];
        }
    }
}

double BasicGladiatorDetector::gaussian1DLikelihood(double x, double mean, double stdev)
{
    double exponent = x - mean;
    exponent *= -exponent;
    exponent /= (2 * stdev * stdev);
    
    double result = std::exp(exponent);
    result /= (stdev * std::sqrt(2 * M_PI));
    return result;
}

void BasicGladiatorDetector::addProps(std::string featureName,
                                   double* mean, double* stdev,
                                   double* defaultMean, double* defaultStDev,
                                   core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "Net" + featureName + "Mean",
                         "mean relative width of symbol with respect to bin inside",
                         defaultMean[0], mean,
                         0.0, 1.0);

    propSet->addProperty(config, false, "Shield" + featureName + "Mean",
                         "mean relative width of symbol with respect to bin inside",
                         defaultMean[1], mean + 1,
                         0.0, 1.0);

    propSet->addProperty(config, false, "Sword" + featureName + "Mean",
                         "mean relative width of symbol with respect to bin inside",
                         defaultMean[2], mean + 2,
                         0.0, 1.0);

    propSet->addProperty(config, false, "Trident" + featureName + "Mean",
                         "mean relative width of symbol with respect to bin inside",
                         defaultMean[3], mean + 3,
                         0.0, 1.0);

    propSet->addProperty(config, false, "Net" + featureName + "StDev",
                         "stdev relative width of symbol with respect to bin inside",
                         defaultStDev[0], stdev,
                         0.0, 1.0);

    propSet->addProperty(config, false, "Shield" + featureName + "StDev",
                         "stdev relative width of symbol with respect to bin inside",
                         defaultStDev[1], stdev + 1,
                         0.0, 1.0);

    propSet->addProperty(config, false, "Sword" + featureName + "StDev",
                         "stdev relative width of symbol with respect to bin inside",
                         defaultStDev[2], stdev + 2,
                         0.0, 1.0);

    propSet->addProperty(config, false, "Trident" + featureName + "StDev",
                         "stdev relative width of symbol with respect to bin inside",
                         defaultStDev[3], stdev + 3,
                         0.0, 1.0);

}

} // namespace vision
} // namespace ram

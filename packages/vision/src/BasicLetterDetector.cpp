/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/BasicLetterDetector.cpp
 */

// STD Includes
#include <iostream>

// Project Includes
#include "vision/include/BasicLetterDetector.h"
#include "vision/include/FANNLetterDetector.h"
#include "vision/include/Symbol.h"
#include "core/include/PropertySet.h"
#include "vision/include/Image.h"

namespace ram {
namespace vision {


double BasicLetterDetector::RELATIVE_SYMBOL_WIDTH_MEAN[BASIC_SYMBOL_COUNT] =
{0.651, 0.566, 0.656, 0.514};
double BasicLetterDetector::RELATIVE_SYMBOL_WIDTH_STDEV[BASIC_SYMBOL_COUNT] = 
{0.078, 0.085, 0.059, 0.067};

double BasicLetterDetector::RELATIVE_SYMBOL_HEIGHT_MEAN[BASIC_SYMBOL_COUNT] = 
{0.447, 0.301, 0.411, 0.307};
double BasicLetterDetector::RELATIVE_SYMBOL_HEIGHT_STDEV[BASIC_SYMBOL_COUNT] = 
{0.054, 0.013, 0.013, 0.035};

double BasicLetterDetector::PIXEL_PERCENTAGE_MEAN[BASIC_SYMBOL_COUNT] = 
{0.122, 0.090, 0.1348, 0.0935};
double BasicLetterDetector::PIXEL_PERCENTAGE_STDEV[BASIC_SYMBOL_COUNT] = 
{0.018, 0.014, 0.0145, 0.0145};

double BasicLetterDetector::CENTER_PIXEL_PERCENTAGE_MEAN[BASIC_SYMBOL_COUNT] =
{240, 245, 0, 4.7};
double BasicLetterDetector::CENTER_PIXEL_PERCENTAGE_STDEV[BASIC_SYMBOL_COUNT] =
{51, 42, 10, 29};

BasicLetterDetector::BasicLetterDetector(core::ConfigNode config,
                                         core::EventHubPtr eventHub) :
    SymbolDetector(eventHub)
{
    m_fannDetector = FANNSymbolDetectorPtr(
        new FANNLetterDetector(core::ConfigNode::fromString("{'training':1}")));
    assert(BASIC_FEATURE_COUNT == m_fannDetector->getNumberFeatures());


    addProps("RelativeWidth", m_relativeSymbolWidthMean, m_relativeSymbolWidthStDev,
             RELATIVE_SYMBOL_WIDTH_MEAN, RELATIVE_SYMBOL_WIDTH_STDEV, config);

    addProps("RelativeHeight", m_relativeSymbolHeightMean, m_relativeSymbolHeightStDev,
             RELATIVE_SYMBOL_HEIGHT_MEAN, RELATIVE_SYMBOL_HEIGHT_STDEV, config);

    addProps("PixelPercentage", m_pixelPercentageMean, m_pixelPercentageStDev,
             PIXEL_PERCENTAGE_MEAN, PIXEL_PERCENTAGE_STDEV, config);

    addProps("CenterPixelPercentage", m_centerPixelPercentageMean,
             m_centerPixelPercentageStDev, CENTER_PIXEL_PERCENTAGE_MEAN,
             CENTER_PIXEL_PERCENTAGE_STDEV, config);
}

void BasicLetterDetector::processImage(Image *input, Image *output)
{
    // Assume we don't know what it is
    m_symbol = Symbol::NONEFOUND;
    
    // Get the features from the image
    float features[BASIC_FEATURE_COUNT];
    m_fannDetector->getImageFeatures(input, features);

    double symbolLikelihood[BASIC_SYMBOL_COUNT] = {1};
    getLikelihoodOfSymbol(features, symbolLikelihood);

    // std::cout << "P(X) = " << symbolLikelihood[0] << std::endl;
    // std::cout << "P(x) = " << symbolLikelihood[1] << std::endl;
    // std::cout << "P(O) = " << symbolLikelihood[2] << std::endl;
    // std::cout << "P(o) = " << symbolLikelihood[3] << std::endl;

    double maxLikelihood = symbolLikelihood[0];
    m_symbol = Symbol::LARGE_X;

    if(symbolLikelihood[1] > maxLikelihood)
    {
        maxLikelihood = symbolLikelihood[1];
        m_symbol = Symbol::SMALL_X;
    }

    if(symbolLikelihood[2] > maxLikelihood)
    {
        maxLikelihood = symbolLikelihood[2];
        m_symbol = Symbol::LARGE_O;
    }

    if(symbolLikelihood[3] > maxLikelihood)
    {
        maxLikelihood = symbolLikelihood[3];
        m_symbol = Symbol::SMALL_O;
    }
    
    if(maxLikelihood < 0.01)
        m_symbol = Symbol::UNKNOWN;
}

void BasicLetterDetector::getLikelihoodOfSymbol(float* features, double* resultLikelihood)
{
    double likelihood[BASIC_SYMBOL_COUNT][BASIC_FEATURE_COUNT] = {{0}};

    for(int symIdx = 0; symIdx < BASIC_SYMBOL_COUNT; symIdx++)
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
    }

    double totalFeatureLikelihood[BASIC_FEATURE_COUNT] = {0};
    
    for(int f = 0; f < BASIC_FEATURE_COUNT; f++)
    {
        for(int s = 0; s < BASIC_SYMBOL_COUNT; s++)
        {
            totalFeatureLikelihood[f] += likelihood[s][f];
        }
    }

    for(int s = 0; s < BASIC_SYMBOL_COUNT; s++)
    {
        resultLikelihood[s] = 1;
    }
    
    for(int f = 0; f < BASIC_FEATURE_COUNT; f++)
    {
        for(int s = 0; s < BASIC_SYMBOL_COUNT; s++)
        {
            likelihood[s][f] /= totalFeatureLikelihood[f];
            resultLikelihood[s] *= likelihood[s][f];
        }
    }
}

double BasicLetterDetector::gaussian1DLikelihood(double x, double mean, double stdev)
{
    double exponent = x - mean;
    exponent *= -exponent;
    exponent /= (2 * stdev * stdev);
    
    double result = std::exp(exponent);
    result /= (stdev * std::sqrt(2 * M_PI));
    return result;
}

void BasicLetterDetector::addProps(std::string featureName,
                                   double* mean, double* stdev,
                                   double* defaultMean, double* defaultStDev,
                                   core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "largeX" + featureName + "Mean",
                         "mean relative width of symbol with respect to bin inside",
                         defaultMean[0], mean,
                         0.0, 1.0);

    propSet->addProperty(config, false, "smallX" + featureName + "Mean",
                         "mean relative width of symbol with respect to bin inside",
                         defaultMean[1], mean + 1,
                         0.0, 1.0);

    propSet->addProperty(config, false, "largeO" + featureName + "Mean",
                         "mean relative width of symbol with respect to bin inside",
                         defaultMean[2], mean + 2,
                         0.0, 1.0);

    propSet->addProperty(config, false, "smallO" + featureName + "Mean",
                         "mean relative width of symbol with respect to bin inside",
                         defaultMean[3], mean + 3,
                         0.0, 1.0);

    propSet->addProperty(config, false, "largeX" + featureName + "StDev",
                         "stdev relative width of symbol with respect to bin inside",
                         defaultStDev[0], stdev,
                         0.0, 1.0);

    propSet->addProperty(config, false, "smallX" + featureName + "StDev",
                         "stdev relative width of symbol with respect to bin inside",
                         defaultStDev[1], stdev + 1,
                         0.0, 1.0);

    propSet->addProperty(config, false, "largeO" + featureName + "StDev",
                         "stdev relative width of symbol with respect to bin inside",
                         defaultStDev[2], stdev + 2,
                         0.0, 1.0);

    propSet->addProperty(config, false, "smallO" + featureName + "StDev",
                         "stdev relative width of symbol with respect to bin inside",
                         defaultStDev[3], stdev + 3,
                         0.0, 1.0);

}

} // namespace vision
} // namespace ram

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
#include "core/include/PropertySet.h"

namespace ram {
namespace vision {

double BasicLetterDetector::LARGE_X_DEFAULTS_MIN[BASIC_FEATURE_COUNT] =
{1.28, 0.29, 0.008, 5.8e-5, 9.7e-7, -4.3e-8, -1.5e-5, 1.02, 0, 0.24};
double BasicLetterDetector::LARGE_X_DEFAULTS_MAX[BASIC_FEATURE_COUNT] =
{1.99, 0.33, 0.016, 0.0015, 0.00028, 2.5e-8, 6.0e-6, 1.73, 0.0106, 0.34};

double BasicLetterDetector::SMALL_X_DEFAULTS_MIN[BASIC_FEATURE_COUNT] =
{1.63, 0.25, 0.0028, 1.99e-5, 7.5e-7, -1.85e-9, -4.3e-6, 1.0, 0, 0.24};
double BasicLetterDetector::SMALL_X_DEFAULTS_MAX[BASIC_FEATURE_COUNT] =
{2.23, 0.27, 0.0053, 0.00082, 0.00015, 7.3e-9, 1.4e-6, 1.9, 0.003, 0.36};
    
double BasicLetterDetector::LARGE_O_DEFAULTS_MIN[BASIC_FEATURE_COUNT] =
{1.38, 0.32, 0.0002, 2.6e-6, 1.28e-6, -2.15e-7, -3.1e-5, 1, 0, 0.22};
double BasicLetterDetector::LARGE_O_DEFAULTS_MAX[BASIC_FEATURE_COUNT] =
{2.1, 0.39, 0.0077, 0.0011, 0.00059, 5.1e-9, 8.0e-7, 1.3, 0.0025, 0.32};

double BasicLetterDetector::SMALL_O_DEFAULTS_MIN[BASIC_FEATURE_COUNT] =
{1.29, 0.25, 4.9e-6, 8.7e-7, 2.25e-6, -1.3e-9, -8.0e-7, 1., 0, 0.26};
double BasicLetterDetector::SMALL_O_DEFAULTS_MAX[BASIC_FEATURE_COUNT] =
{2, 0.29, 0.0011, 0.00034, 0.00017, 7.22e-9, 4.6e-7, 1.8, 0.0015, 0.36};

BasicLetterDetector::BasicLetterDetector(core::ConfigNode config,
                                         core::EventHubPtr eventHub) :
    SymbolDetector(eventHub)
{
    m_fannDetector = FANNSymbolDetectorPtr(
        new FANNLetterDetector(core::ConfigNode::fromString("{'training':1}")));
    assert(BASIC_FEATURE_COUNT == m_fannDetector->getNumberFeatures());

    // General properties
    addMinMaxProps("LargeX", m_largeXFeaturesMin, m_largeXFeaturesMax,
                   LARGE_X_DEFAULTS_MIN, LARGE_X_DEFAULTS_MAX, config); 
    addMinMaxProps("SmallX", m_smallXFeaturesMin, m_smallXFeaturesMax,
                   SMALL_X_DEFAULTS_MIN, SMALL_X_DEFAULTS_MAX, config);
    addMinMaxProps("LargeO", m_largeOFeaturesMin, m_largeOFeaturesMax,
                   LARGE_O_DEFAULTS_MIN, LARGE_O_DEFAULTS_MAX, config);
    addMinMaxProps("SmallO", m_smallOFeaturesMin, m_smallOFeaturesMax,
                   SMALL_O_DEFAULTS_MIN, SMALL_O_DEFAULTS_MAX, config);
}

void BasicLetterDetector::processImage(Image *input, Image *output)
{
    std::cout << "Processing the image" << std::endl;
    // Assume we don't know what it is
    m_symbol = Symbol::NONEFOUND;
    
    // Get the features from the image
    float features[BASIC_FEATURE_COUNT];
    m_fannDetector->getImageFeatures(input, features);

    // Check for the large X
    if (checkSym(features, m_largeXFeaturesMin, m_largeXFeaturesMax))
        m_symbol = Symbol::LARGE_X;
    
    // Now for the small X
    if (checkSym(features, m_smallXFeaturesMin, m_smallXFeaturesMax))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::SMALL_X;
        else
            m_symbol = Symbol::UNKNOWN;
    }
    
    // Now for the large O
    if (checkSym(features, m_largeOFeaturesMin, m_largeOFeaturesMax)
        && (m_symbol != Symbol::UNKNOWN))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::LARGE_O;
        else
            m_symbol = Symbol::UNKNOWN;
    }
    
    // Now for the small O
    if (checkSym(features, m_smallOFeaturesMin, m_smallOFeaturesMax)
        && (m_symbol != Symbol::UNKNOWN))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::SMALL_O;
        else
            m_symbol = Symbol::UNKNOWN;
    }

    
    // If we didn't find anything its unknown
    if (m_symbol == Symbol::NONEFOUND)
        m_symbol = Symbol::UNKNOWN;

    std::cout << std::endl;
}

bool BasicLetterDetector::checkSym(float* incomingFeatures, double* minFeatures,
                                   double* maxFeatures)
{
    int numMisses = 0;
    // Break out of the values: [aspectRatio, sideFillRatio, cornerFillAvg]
    for (int i = 0; i < BASIC_FEATURE_COUNT; ++i)
    {
        double value = incomingFeatures[i];
        if ((value < minFeatures[i]) || (value > maxFeatures[i]))
            numMisses++;
    }
    
    std::cout << " Num Misses: " << numMisses;
    if(numMisses < 5)
        return true;
    else
        return false;
}

void BasicLetterDetector::addMinMaxProps(std::string name, double* minFeatures,
                                         double* maxFeatures,
                                         double* minDefaults,
                                         double* maxDefaults,
                                         core::ConfigNode config)
{
    addFeatureProp("min", name, "Min", minFeatures, minDefaults, config);
    addFeatureProp("max", name, "Max", maxFeatures, maxDefaults, config);
}

void BasicLetterDetector::addFeatureProp(std::string prefix, std::string suffix,
                                         std::string name,
                                         double* features, double* defaults,
                                         core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());
   
    propSet->addProperty(config, false, "letter" + name + "AspectRatio" + suffix,
                         prefix + " aspect ratio for symbol to be a " + name,
                         defaults[0], features, 1.0, 2.0);
    propSet->addProperty(config, false, "letter" + name + "SideFillRatio" + suffix,
                         prefix + " side Fill ratio for symbol to be a " + name,
                         defaults[1], features + 1, 1.0, 10.0);
    propSet->addProperty(config, false, "letter" + name + "Hu1" + suffix,
                         prefix + "Hu1" + name,
                         defaults[2], features + 2, 0.0, 1.0/*0.0, 0.01*/);
    propSet->addProperty(config, false, "letter" + name + "Hu2" + suffix,
                         prefix + "Hu2" + name,
                         defaults[3], features + 3, 0.0, 1.0/* 0.0, 0.02*/);
    propSet->addProperty(config, false, "letter" + name + "Hu3" + suffix,
                         prefix + "Hu3" + name,
                         defaults[4], features + 4, 0.0, 1.0/*0.0, 0.01*/);
    propSet->addProperty(config, false, "letter" + name + "Hu4" + suffix,
                         prefix + "Hu4" + name,
                         defaults[5], features + 5, -1.0, 1.0/*-4e-6, 4e-6*/);
    propSet->addProperty(config, false, "letter" + name + "Hu5" + suffix,
                         prefix + "Hu5" + name,
                         defaults[6], features + 6, -1.0, 1.0/*-5e-4, 5e-4*/);
    propSet->addProperty(config, false, "letter" + name + "Hu6" + suffix,
                         prefix + "Hu6" + name,
                         defaults[7], features + 7, 0.0, 5.0/*0.5, 3.0*/);
    propSet->addProperty(config, false, "letter" + name + "CornerFillAvg" + suffix,
                         prefix + " corner fill aevrage ratio for symbol to be a " + name,
                         defaults[9], features + 8, 0.0, 1.0);
    propSet->addProperty(config, false, "letter" + name + "MiddleFillAvg" + suffix,
                         prefix + " middle fill average ratio for symbol to be a " + name,
                         defaults[10], features + 9, 0.0, 1.0);
}

} // namespace vision
} // namespace ram

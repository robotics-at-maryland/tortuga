/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/BasicLetterDetector.cpp
 */

// Project Includes
#include "vision/include/BasicLetterDetector.h"
#include "vision/include/FANNLetterDetector.h"

#include "core/include/PropertySet.h"

namespace ram {
namespace vision {

double BasicLetterDetector::LARGE_X_DEFAULTS_MIN[FEATURE_COUNT] =
    {3.51, 8.93, 0.26, 0.13};
double BasicLetterDetector::LARGE_X_DEFAULTS_MAX[FEATURE_COUNT] =
    {4.86, 1.01, 0.49, 0.20};

double BasicLetterDetector::SMALL_X_DEFAULTS_MIN[FEATURE_COUNT] =
    {2.12, 1.09, 0.00, 0.14};
double BasicLetterDetector::SMALL_X_DEFAULTS_MAX[FEATURE_COUNT] =
    {3.39, 3.67, 0.04, 0.22};

double BasicLetterDetector::LARGE_O_DEFAULTS_MIN[FEATURE_COUNT] =
    {3.66, 1.02, 0.00, 0.15};
double BasicLetterDetector::LARGE_O_DEFAULTS_MAX[FEATURE_COUNT] =
    {4.86, 7.65, 0.24, 0.21};

double BasicLetterDetector::SMALL_O_DEFAULTS_MIN[FEATURE_COUNT] =
    {3.71, 2.48, 0.03, 0.25};
double BasicLetterDetector::SMALL_O_DEFAULTS_MAX[FEATURE_COUNT] =
    {7.64, 8.57, 0.45, 0.51};

BasicLetterDetector::BasicLetterDetector(core::ConfigNode config,
                                         core::EventHubPtr eventHub) :
    SymbolDetector(eventHub)
{
    m_fannDetector = FANNSymbolDetectorPtr(
        new FANNLetterDetector(core::ConfigNode::fromString("{'training':1}")));
    assert(FEATURE_COUNT == m_fannDetector->getNumberFeatures());

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
    // Assume we don't know what it is
    m_symbol = Symbol::NONEFOUND;
    
    // Get the features from the image
    float features[FEATURE_COUNT];
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
}

bool BasicLetterDetector::checkSym(float* incomingFeatures, double* minFeatures,
                                   double* maxFeatures)
{
    bool matches = true;
    
    // Break out of the values: [aspectRatio, sideFillRatio, cornerFillAvg]
    for (int i = 0; i < FEATURE_COUNT; ++i)
    {
        double value = incomingFeatures[i];
        if ((value < minFeatures[i]) || (value > maxFeatures[i]))
            matches = false;
    }
    
    return matches;
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
    propSet->addProperty(config, false, "letter" + name + "CornerFillAvg" + suffix,
        prefix + " corner fill aevrage ratio for symbol to be a " + name,
        defaults[2], features + 2, 0.0, 1.0);
    propSet->addProperty(config, false, "letter" + name + "MiddleFillAvg" + suffix,
        prefix + " middle fill average ratio for symbol to be a " + name,
        defaults[3], features + 3, 0.0, 1.0);
}

} // namespace vision
} // namespace ram

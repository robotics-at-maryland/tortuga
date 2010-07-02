/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/BasicWeaponDetector.cpp
 */

// Project Includes
#include "vision/include/BasicWeaponDetector.h"
#include "vision/include/FANNWeaponDetector.h"

#include "core/include/PropertySet.h"

namespace ram {
namespace vision {

double BasicWeaponDetector::AXE_DEFAULTS_MIN[FEATURE_COUNT] =
    {3.51, 8.93, 0.26, 0.13};
double BasicWeaponDetector::AXE_DEFAULTS_MAX[FEATURE_COUNT] =
    {4.86, 1.01, 0.49, 0.20};

double BasicWeaponDetector::CLIPPERS_DEFAULTS_MIN[FEATURE_COUNT] =
    {2.12, 1.09, 0.00, 0.14};
double BasicWeaponDetector::CLIPPERS_DEFAULTS_MAX[FEATURE_COUNT] =
    {3.39, 3.67, 0.04, 0.22};

double BasicWeaponDetector::HAMMER_DEFAULTS_MIN[FEATURE_COUNT] =
    {3.66, 1.02, 0.00, 0.15};
double BasicWeaponDetector::HAMMER_DEFAULTS_MAX[FEATURE_COUNT] =
    {4.86, 7.65, 0.24, 0.21};

double BasicWeaponDetector::MACHETE_DEFAULTS_MIN[FEATURE_COUNT] =
    {3.71, 2.48, 0.03, 0.25};
double BasicWeaponDetector::MACHETE_DEFAULTS_MAX[FEATURE_COUNT] =
    {7.64, 8.57, 0.45, 0.51};

BasicWeaponDetector::BasicWeaponDetector(core::ConfigNode config,
                                         core::EventHubPtr eventHub) :
    SymbolDetector(eventHub)
{
    m_fannDetector = FANNSymbolDetectorPtr(
        new FANNWeaponDetector(core::ConfigNode::fromString("{'training':1}")));
    assert(FEATURE_COUNT == m_fannDetector->getNumberFeatures());

    // General properties
    addMinMaxProps("Axe", m_axeFeaturesMin, m_axeFeaturesMax,
                   AXE_DEFAULTS_MIN, AXE_DEFAULTS_MAX, config);
    addMinMaxProps("Clippers", m_clippersFeaturesMin, m_clippersFeaturesMax,
                   CLIPPERS_DEFAULTS_MIN, CLIPPERS_DEFAULTS_MAX, config);
    addMinMaxProps("Hammer", m_hammerFeaturesMin, m_hammerFeaturesMax,
                   HAMMER_DEFAULTS_MIN, HAMMER_DEFAULTS_MAX, config);
    addMinMaxProps("Machete", m_macheteFeaturesMin, m_macheteFeaturesMax,
                   MACHETE_DEFAULTS_MIN, MACHETE_DEFAULTS_MAX, config);
}

void BasicWeaponDetector::processImage(Image *input, Image *output)
{
    // Assume we don't know what it is
    m_symbol = Symbol::NONEFOUND;
    
    // Get the features from the image
    float features[FEATURE_COUNT];
    m_fannDetector->getImageFeatures(input, features);

    // Check for the axe
    if (checkSym(features, m_axeFeaturesMin, m_axeFeaturesMax))
        m_symbol = Symbol::AXE;
    
    // Now for the clippers
    if (checkSym(features, m_clippersFeaturesMin, m_clippersFeaturesMax))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::CLIPPERS;
        else
            m_symbol = Symbol::UNKNOWN;
    }
    
    // Now for the hammer
    if (checkSym(features, m_hammerFeaturesMin, m_hammerFeaturesMax)
        && (m_symbol != Symbol::UNKNOWN))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::HAMMER;
        else
            m_symbol = Symbol::UNKNOWN;
    }
    
    // Now for the machete
    if (checkSym(features, m_macheteFeaturesMin, m_macheteFeaturesMax)
        && (m_symbol != Symbol::UNKNOWN))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::MACHETE;
        else
            m_symbol = Symbol::UNKNOWN;
    }

    
    // If we didn't find anything its unknown
    if (m_symbol == Symbol::NONEFOUND)
        m_symbol = Symbol::UNKNOWN;
}

bool BasicWeaponDetector::checkSym(float* incomingFeatures, double* minFeatures,
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

void BasicWeaponDetector::addMinMaxProps(std::string name, double* minFeatures,
                                         double* maxFeatures,
                                         double* minDefaults,
                                         double* maxDefaults,
                                         core::ConfigNode config)
{
    addFeatureProp("min", name, "Min", minFeatures, minDefaults, config);
    addFeatureProp("max", name, "Max", maxFeatures, maxDefaults, config);
}

void BasicWeaponDetector::addFeatureProp(std::string prefix, std::string suffix,
                                         std::string name,
                                         double* features, double* defaults,
                                         core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());
   
    propSet->addProperty(config, false, "weapon" + name + "AspectRatio" + suffix,
        prefix + " aspect ratio for symbol to be a " + name,
        defaults[0], features, 1.0, 2.0);
    propSet->addProperty(config, false, "weapon" + name + "SideFillRatio" + suffix,
        prefix + " side Fill ratio for symbol to be a " + name,
        defaults[1], features + 1, 1.0, 10.0);
    propSet->addProperty(config, false, "weapon" + name + "CornerFillAvg" + suffix,
        prefix + " corner fill aevrage ratio for symbol to be a " + name,
        defaults[2], features + 2, 0.0, 1.0);
    propSet->addProperty(config, false, "weapon" + name + "MiddleFillAvg" + suffix,
        prefix + " middle fill average ratio for symbol to be a " + name,
        defaults[3], features + 3, 0.0, 1.0);
}

} // namespace vision
} // namespace ram

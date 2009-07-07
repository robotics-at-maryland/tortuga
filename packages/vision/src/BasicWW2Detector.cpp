/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/BasicWW2Detector.cpp
 */

// Project Includes
#include "vision/include/BasicWW2Detector.h"
#include "vision/include/FANNWW2Detector.h"

#include "core/include/PropertySet.h"


namespace ram {
namespace vision {

// Average: 2.43604 7.9089 0.400077    
double BasicWW2Detector::SHIP_DEFAULTS_MIN[3] = {1.5, 5.0, 0.3};
double BasicWW2Detector::SHIP_DEFAULTS_MAX[3] = {3.0, 10.0, 0.6};

// Average: 1.06201 2.52522 0    
double BasicWW2Detector::AIRCRAFT_DEFAULTS_MIN[3] = {1.0, 2.25, 0.0};
double BasicWW2Detector::AIRCRAFT_DEFAULTS_MAX[3] = {1.2, 2.75, 0.25};

// Average: 2.35522 1.55058 0.0508255
double BasicWW2Detector::TANK_DEFAULTS_MIN[3] = {1.75, 1.3, 0.0};
double BasicWW2Detector::TANK_DEFAULTS_MAX[3] = {2.75, 1.7, 0.25};

// Average: 2.04122 2.01597 0.512011    
double BasicWW2Detector::FACTORY_DEFAULTS_MIN[3] = {1.8, 1.8, 0.4};
double BasicWW2Detector::FACTORY_DEFAULTS_MAX[3] = {2.2, 2.2, 0.7};

    
BasicWW2Detector::BasicWW2Detector(core::ConfigNode config,
                                   core::EventHubPtr eventHub) :
    SymbolDetector(eventHub)
{
    // Create the detector we use to find image features, and make sure it
    // produces the right number of features
    m_fannDetector = FANNSymbolDetectorPtr(
        new FANNWW2Detector(core::ConfigNode::fromString("{'training' : 1}")));
    assert(3 == m_fannDetector->getNumberFeatures());

    // General properties
    addMinMaxProps("Ship", m_shipFeaturesMin, m_shipFeaturesMax,
                   SHIP_DEFAULTS_MIN, SHIP_DEFAULTS_MAX, config);
    addMinMaxProps("Aircraft", m_aircraftFeaturesMin, m_aircraftFeaturesMax,
                   AIRCRAFT_DEFAULTS_MIN, AIRCRAFT_DEFAULTS_MAX, config);
    addMinMaxProps("Tank", m_tankFeaturesMin, m_tankFeaturesMax,
                   TANK_DEFAULTS_MIN, TANK_DEFAULTS_MAX, config);
    addMinMaxProps("Factory", m_factoryFeaturesMin, m_factoryFeaturesMax,
                   FACTORY_DEFAULTS_MIN, FACTORY_DEFAULTS_MAX, config);
}
    
void BasicWW2Detector::processImage(Image* input, Image* output)
{
    // Assume we don't know what it is
    m_symbol = Symbol::NONEFOUND;
    
    // Get the features from the image
    float features[3];
    m_fannDetector->getImageFeatures(input, features);

    // Check for the ship
    if (checkSym(features, m_shipFeaturesMin, m_shipFeaturesMax))
        m_symbol = Symbol::SHIP;
    
    // Now for the aircraft
    if (checkSym(features, m_aircraftFeaturesMin, m_aircraftFeaturesMax))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::AIRCRAFT;
        else
            m_symbol = Symbol::UNKNOWN;
    }
    
    // Now for the tank
    if (checkSym(features, m_tankFeaturesMin, m_tankFeaturesMax)
        && (m_symbol != Symbol::UNKNOWN))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::TANK;
        else
            m_symbol = Symbol::UNKNOWN;
    }
    
    // Now for the factory
    if (checkSym(features, m_factoryFeaturesMin, m_factoryFeaturesMax)
        && (m_symbol != Symbol::UNKNOWN))
    {
        if (m_symbol == Symbol::NONEFOUND)
            m_symbol = Symbol::FACTORY;
        else
            m_symbol = Symbol::UNKNOWN;
    }

    
    // If we didn't find anything its unknown
    if (m_symbol == Symbol::NONEFOUND)
        m_symbol = Symbol::UNKNOWN;
}

bool BasicWW2Detector::checkSym(float* incomingFeatures, double* minFeatures,
                                double* maxFeatures)
{
    bool matches = true;
    
    // Break out of the values: [aspectRatio, sideFillRatio, cornerFillAvg]
    for (int i = 0; i < 3; ++i)
    {
        double value = incomingFeatures[i];
        if ((value < minFeatures[i]) || (value > maxFeatures[i]))
            matches = false;
    }
    
    return matches;
}

void BasicWW2Detector::addMinMaxProps(std::string name, double* minFeatures,
                                      double* maxFeatures, double* minDefaults,
                                      double* maxDefaults,
                                      core::ConfigNode config)
{
    addFeatureProp("min", name, "Min", minFeatures, minDefaults, config);
    addFeatureProp("max", name, "Max", maxFeatures, maxDefaults, config);
}

void BasicWW2Detector::addFeatureProp(std::string prefix, std::string suffix,
                                      std::string name,
                                      double* features, double* defaults,
                                      core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());
   
    propSet->addProperty(config, false, "ww2" + name + "AspectRatio" + suffix,
        prefix + " aspect ratio for symbol to be a " + name,
        defaults[0], features, 1.0, 2.0);
    propSet->addProperty(config, false, "ww2" + name + "SideFillRatio" + suffix,
        prefix + " side Fill ratio for symbol to be a " + name,
        defaults[1], features + 1, 1.0, 10.0);
    propSet->addProperty(config, false, "ww2" + name + "CornerFillAvg" + suffix,
        prefix + " corner fill average ratio for symbol to be a " + name,
        defaults[2], features + 2, 0.0, 1.0);
}
    
} // namespace vision
} // namespace ram

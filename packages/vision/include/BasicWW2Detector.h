/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/BasicWW2Detector.h
 */

#ifndef RAM_VISION_BasicWW2DETECTOR_H_07_06_2009
#define RAM_VISION_BasicWW2DETECTOR_H_07_06_2009

// Project Includes
#include "vision/include/SymbolDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** Implements SymbolDetector to find the 2009 Symbols
 *
 *  The features are a the amount of red in the four corners of the image, the
 *  aspect ratio of the image, and the amount of red on the two half of the
 *  image split the longest way.
 */
class RAM_EXPORT BasicWW2Detector : public SymbolDetector
{
  public:
    BasicWW2Detector(core::ConfigNode config,
                     core::EventHubPtr eventHub = core::EventHubPtr());

    // Detector Methods
    void processImage(Image* input, Image* output= 0);
        
    // SymbolDetector Methods
    virtual Symbol::SymbolType getSymbol() { return m_symbol; }
    virtual bool needSquareCropped() { return false; }
    
  private:
    bool checkSym(float* incomingFeatures, double* minFeatures,
                  double* maxFeatures);
    
    void addMinMaxProps(std::string name, double* minFeatures,
                        double* maxFeatures, double* minDefaults,
                        double* maxDefaults, core::ConfigNode config);

    void addFeatureProp(std::string prefix, std::string name,
                        std::string suffix, double* features,
                        double* defaults, core::ConfigNode config);
    
    
    /** Slight hack, but we use this to get the features of the image */
    FANNSymbolDetectorPtr m_fannDetector;

    /** Out current symbol */
    Symbol::SymbolType m_symbol;
    
    double m_shipFeaturesMin[3];
    double m_shipFeaturesMax[3];

    double m_aircraftFeaturesMin[3];
    double m_aircraftFeaturesMax[3];
       
    double m_tankFeaturesMin[3];
    double m_tankFeaturesMax[3];

    double m_factoryFeaturesMin[3];
    double m_factoryFeaturesMax[3];

    static double SHIP_DEFAULTS_MIN[3];
    static double SHIP_DEFAULTS_MAX[3];
    
    static double AIRCRAFT_DEFAULTS_MIN[3];
    static double AIRCRAFT_DEFAULTS_MAX[3];

    static double TANK_DEFAULTS_MIN[3];
    static double TANK_DEFAULTS_MAX[3];

    static double FACTORY_DEFAULTS_MIN[3];
    static double FACTORY_DEFAULTS_MAX[3];
};
	
} // namespace vision
} // namespace ram

#endif // RAM_VISION_BasicWW2DETECTOR_H_07_06_2009

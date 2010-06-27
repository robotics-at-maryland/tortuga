/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/BasicWeaponDetector.h
 */

// Project Includes
#include "vision/include/SymbolDetector.h"

// Must be included last
#include "vision/include/Export.h"

#ifndef RAM_VISION_BASICWEAPONDETECTOR_H_06_27_2010
#define RAM_VISION_BASICWEAPONDETECTOR_H_06_27_2010

namespace ram {
namespace vision {

#define FEATURE_COUNT 4

/** Implements the SymbolDetector to find the 2010 symbols.
 *
 *  The features are the amount of red in the four corners of the image, the
 *  aspect ratio of the image, and the amount of red on the two halves of the
 *  image split the shortest way.
 */
class RAM_EXPORT BasicWeaponDetector : public SymbolDetector
{
  public:
    BasicWeaponDetector(core::ConfigNode config,
                        core::EventHubPtr eventHub = core::EventHubPtr());

    // Detector Methods
    void processImage(Image* input, Image* output = 0);

    // SymbolDetector Methods
    virtual Symbol::SymbolType getSymbol() { return m_symbol; }
    virtual bool needSquareCropped() { return false; }

  private:
    bool checkSym(float* incomingFeatures, double* minFeatures,
                  double* maxFeatures);

    void addMinMaxProps(std::string name, double* minFeatures,
                        double* maxFeatures, double* minDefaults,
                        double* maxDefaults, core::ConfigNode config);

    void addFeatureProp(std::string prefix, std::string suffix,
                        std::string name, double* features, double* defaults,
                        core::ConfigNode config);

    FANNSymbolDetectorPtr m_fannDetector;

    /** Our current symbol */
    Symbol::SymbolType m_symbol;

    double m_axeFeaturesMin[FEATURE_COUNT];
    double m_axeFeaturesMax[FEATURE_COUNT];

    double m_clippersFeaturesMin[FEATURE_COUNT];
    double m_clippersFeaturesMax[FEATURE_COUNT];

    double m_hammerFeaturesMin[FEATURE_COUNT];
    double m_hammerFeaturesMax[FEATURE_COUNT];

    double m_macheteFeaturesMin[FEATURE_COUNT];
    double m_macheteFeaturesMax[FEATURE_COUNT];

    static double AXE_DEFAULTS_MIN[FEATURE_COUNT];
    static double AXE_DEFAULTS_MAX[FEATURE_COUNT];

    static double CLIPPERS_DEFAULTS_MIN[FEATURE_COUNT];
    static double CLIPPERS_DEFAULTS_MAX[FEATURE_COUNT];

    static double HAMMER_DEFAULTS_MIN[FEATURE_COUNT];
    static double HAMMER_DEFAULTS_MAX[FEATURE_COUNT];

    static double MACHETE_DEFAULTS_MIN[FEATURE_COUNT];
    static double MACHETE_DEFAULTS_MAX[FEATURE_COUNT];
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_BASICWEAPONDETECTOR_H_06_27_2010

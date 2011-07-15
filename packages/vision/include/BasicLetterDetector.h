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

#ifndef RAM_VISION_BASICLETTERDETECTOR_H_06_27_2010
#define RAM_VISION_BASICLETTERDETECTOR_H_06_27_2010

namespace ram {
namespace vision {

#define BASIC_SYMBOL_COUNT 4
#define BASIC_FEATURE_COUNT 4

/** Implements the SymbolDetector to find the 2010 symbols.
 *
 *  The features are the amount of red in the four corners of the image, the
 *  aspect ratio of the image, and the amount of red on the two halves of the
 *  image split the shortest way.
 */
class RAM_EXPORT BasicLetterDetector : public SymbolDetector
{
  public:
    BasicLetterDetector(core::ConfigNode config,
                        core::EventHubPtr eventHub = core::EventHubPtr());

    // Detector Methods
    void processImage(Image* input, Image* output = 0);

    // SymbolDetector Methods
    virtual Symbol::SymbolType getSymbol() { return m_symbol; }
    virtual bool needSquareCropped() { return false; }

  private:
    void addProps(std::string featureName, double* mean, double* stdev,
                  double *defaultMean, double* defaultStDev,
                  core::ConfigNode config);

    void getLikelihoodOfSymbol(float* features, double* resultLikelihood);

    double gaussian1DLikelihood(double x, double mean, double stdev);

    FANNSymbolDetectorPtr m_fannDetector;

    /** Our current symbol */
    Symbol::SymbolType m_symbol;

    double m_relativeSymbolWidthMean[BASIC_SYMBOL_COUNT];
    double m_relativeSymbolWidthStDev[BASIC_SYMBOL_COUNT];
    
    double m_relativeSymbolHeightMean[BASIC_SYMBOL_COUNT];
    double m_relativeSymbolHeightStDev[BASIC_SYMBOL_COUNT];

    double m_pixelPercentageMean[BASIC_SYMBOL_COUNT];
    double m_pixelPercentageStDev[BASIC_SYMBOL_COUNT];
        
    double m_centerPixelPercentageMean[BASIC_SYMBOL_COUNT];
    double m_centerPixelPercentageStDev[BASIC_SYMBOL_COUNT];

    static double RELATIVE_SYMBOL_WIDTH_MEAN[BASIC_SYMBOL_COUNT];
    static double RELATIVE_SYMBOL_WIDTH_STDEV[BASIC_SYMBOL_COUNT];

    static double RELATIVE_SYMBOL_HEIGHT_MEAN[BASIC_SYMBOL_COUNT];
    static double RELATIVE_SYMBOL_HEIGHT_STDEV[BASIC_SYMBOL_COUNT];

    static double PIXEL_PERCENTAGE_MEAN[BASIC_SYMBOL_COUNT];
    static double PIXEL_PERCENTAGE_STDEV[BASIC_SYMBOL_COUNT];
    
    static double CENTER_PIXEL_PERCENTAGE_MEAN[BASIC_SYMBOL_COUNT];
    static double CENTER_PIXEL_PERCENTAGE_STDEV[BASIC_SYMBOL_COUNT];
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_BASICLETTERDETECTOR_H_06_27_2010

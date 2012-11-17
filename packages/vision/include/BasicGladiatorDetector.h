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

#ifndef RAM_VISION_BASICGLADIATORDETECTOR_H_06_27_2010
#define RAM_VISION_BASICGLADIATORDETECTOR_H_06_27_2010

namespace ram {
namespace vision {

#define GLADIATOR_BASIC_SYMBOL_COUNT 4
#define GLADIATOR_BASIC_FEATURE_COUNT 6

/** Implements the SymbolDetector to find the 2010 symbols.
 *
 *  The features are the amount of red in the four corners of the image, the
 *  aspect ratio of the image, and the amount of red on the two halves of the
 *  image split the shortest way.
 */
class RAM_EXPORT BasicGladiatorDetector : public SymbolDetector
{

  public:
    BasicGladiatorDetector(core::ConfigNode config,
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

    double m_relativeSymbolWidthMean[GLADIATOR_BASIC_SYMBOL_COUNT];
    double m_relativeSymbolWidthStDev[GLADIATOR_BASIC_SYMBOL_COUNT];
    
    double m_relativeSymbolHeightMean[GLADIATOR_BASIC_SYMBOL_COUNT];
    double m_relativeSymbolHeightStDev[GLADIATOR_BASIC_SYMBOL_COUNT];

    double m_pixelPercentageMean[GLADIATOR_BASIC_SYMBOL_COUNT];
    double m_pixelPercentageStDev[GLADIATOR_BASIC_SYMBOL_COUNT];
        
    double m_centerPixelPercentageMean[GLADIATOR_BASIC_SYMBOL_COUNT];
    double m_centerPixelPercentageStDev[GLADIATOR_BASIC_SYMBOL_COUNT];

    double m_upperDiagonalPixelPercentageMean[GLADIATOR_BASIC_SYMBOL_COUNT];
    double m_upperDiagonalPixelPercentageStDev[GLADIATOR_BASIC_SYMBOL_COUNT];

    double m_lowerDiagonalPixelPercentageMean[GLADIATOR_BASIC_SYMBOL_COUNT];
    double m_lowerDiagonalPixelPercentageStDev[GLADIATOR_BASIC_SYMBOL_COUNT];

    static double RELATIVE_SYMBOL_WIDTH_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT];
    static double RELATIVE_SYMBOL_WIDTH_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT];

    static double RELATIVE_SYMBOL_HEIGHT_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT];
    static double RELATIVE_SYMBOL_HEIGHT_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT];

    static double PIXEL_PERCENTAGE_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT];
    static double PIXEL_PERCENTAGE_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT];
    
    static double CENTER_PIXEL_PERCENTAGE_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT];
    static double CENTER_PIXEL_PERCENTAGE_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT];

    static double UPPER_DIAGONAL_PIXEL_PERCENTAGE_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT];
    static double UPPER_DIAGONAL_PIXEL_PERCENTAGE_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT];

    static double LOWER_DIAGONAL_PIXEL_PERCENTAGE_MEAN[GLADIATOR_BASIC_SYMBOL_COUNT];
    static double LOWER_DIAGONAL_PIXEL_PERCENTAGE_STDEV[GLADIATOR_BASIC_SYMBOL_COUNT];
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_BASICGLADIATORDETECTOR_H_06_27_2010

/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FANNSymbolDetector.h
 */

#ifndef RAM_VISION_FANNSYMBOLDETECTOR_H_07_03_2009
#define RAM_VISION_FANNSYMBOLDETECTOR_H_07_03_2009

// Project Includes
#include "vision/include/SymbolDetector.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace FANN {
    class neural_net;
}

namespace ram {
namespace vision {

/** A basic class for symbol detectors based on Nueral Networks
 *
 *  Subclassing and implementing the getImageFeatures function, and running
 *  with a networked trained using the same function will match images.
 */
class RAM_EXPORT FANNSymbolDetector : public SymbolDetector
{
public:
    /** The number of features this detector extracts from an image */
    int getNumberFeatures();

    /** The number of different symbols the network is differentiating between*/
    int getOutputCount();

    /** Extract a set of floating point features from an image
     *
     *  @param inputImage
     *      The image to extract the features from
     *  @param features
     *      The array to store the features in, must get getNumberOfFeatures
     *      long.
     */
    virtual void getImageFeatures(Image* inputImage, float* features) = 0;

    /** Runs the image through the NN
     *
     *  @return
     *      -1 if no output was high enough to match, otherwise its 0 <= number
     *      < getOutputCount().
     */
    int runNN(Image* input);

    /** The last result returned from runNN */
    int getResult();
    
protected:
    FANNSymbolDetector(int numberOfFeatures, int outputCount,
                       core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());
        
private:
    /** The number of features */
    int m_numberFeatures;
    
    /** The number of outputs */
    int m_outputCount;

    /** Last result */
    int m_result;

    /** The minimum value for an NN output to be considered a match */
    double m_outputThreshold;
    
    /** Features */
    float* m_features;

    /** My nueral network */
    FANN::neural_net* m_net;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_FANNSYMBOLDETECTOR_H_07_03_2009

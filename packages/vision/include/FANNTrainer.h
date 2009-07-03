/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/include/FANNTrainer.h
 */

#ifndef RAM_VISION_FANN_TRAINER_HPP
#define RAM_VISION_FANN_TRAINER_HPP

// standard stuff
#include <vector>
#include <iostream>

// boost file stuff
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

// fann library
#include <floatfann.h>
#include <fann_cpp.h>

// image manipulation
#include <opencv/cv.h>

// ram image header
#include "vision/include/Image.h"
#include "vision/include/Common.h"
#include "core/include/ConfigNode.h"

// thingus
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT FANNTrainer {
  public:
    /** Build a new FANNTrainer **/
    FANNTrainer(const unsigned int outputCount, FANNSymbolDetectorPtr detector,
                core::ConfigNode config =
                ram::core::ConfigNode::fromString("{}"));
        
    /** Train on a given set of data **/
    void runTraining (FANN::training_data &data);
    
    /** Test the FANNTrainer with a set of training data **/
    const void runTest (FANN::training_data &data,
                        std::ostream &out = std::cout);
    /** Save this FANNTrainer to a file **/
    const bool save (const boost::filesystem::path &file);
        
    /** Print a representation of this FANNTrainer's fann network internals **/
    inline const void print () {
        m_net.print_parameters();
    }
    
    /** Add a set of images to a set for training data for use with this
     *  FANNTrainer
     */
    bool addTrainData (unsigned int imageIndex, FANN::training_data &data,
                       std::vector<Image*> &images);
    
    
  private:
    /** Load this network from a file **/
    //void loadFromFile (const boost::filesystem::path &file);
    
    FANN::neural_net m_net;
    bool m_cascade;
    float m_sizeFactor, m_desiredError;
    int m_maxEpochs, m_reportEpochs, m_reportNeurons;
    FANNSymbolDetectorPtr m_fannDetector;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_FANN_TRAINER_HPP

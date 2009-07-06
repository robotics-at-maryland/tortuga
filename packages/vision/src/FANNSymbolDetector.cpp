/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/src/FANNSymboleDetector.h
 */
#include <iostream>

// Library Includes
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <floatfann.h>
#include <fann_cpp.h>

// Project Includes
#include "vision/include/FANNSymbolDetector.h"

#include "core/include/PropertySet.h"

namespace ram {
namespace vision {

int FANNSymbolDetector::getNumberFeatures()
{
    return m_numberFeatures;
}

int FANNSymbolDetector::getOutputCount()
{
    return m_outputCount;
}

int FANNSymbolDetector::runNN(Image* input)
{
    // Grab the features from the image
    getImageFeatures(input, m_features);

    // Run the detector on the features
    fann_type* outValue = m_net->run(m_features);
    
    // Find the highest output of the network
    unsigned int highest_out = 0;
    for (unsigned int i = 0; i < m_net->get_num_output(); ++i)
    {
        if (outValue[i] > outValue[highest_out])
        {
            highest_out = i;
        }
    }
    
    // Determine if its above the threshold or not
    if (outValue[highest_out] > m_outputThreshold)
        m_result = highest_out;
    else
        m_result = -1;
    return m_result;
}

int FANNSymbolDetector::getResult()
{
    return m_result;
}
    

FANNSymbolDetector::FANNSymbolDetector(int numberOfFeatures, int outputCount,
                                       core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    m_numberFeatures(numberOfFeatures),
    m_outputCount(outputCount),
    m_result(-1),
    m_outputThreshold(0),
    m_features(new fann_type[numberOfFeatures]),
    m_net(new FANN::neural_net())
{
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    // General properties
    propSet->addProperty(config, false, "NNoutputThreshold",
        "Minimum value for an output of the nueral network to be matched",
        0.1, &m_outputThreshold, 0.0, 1.0);

    // Get the base configuration file path

    // Get the path from the SVN directory
    if (config["training"].asInt(0) != 1)
    {
        assert(config.exists("nueralNetworkFile") &&
               "Not nueral network file found");
        std::string shortPath(config["nueralNetworkFile"].asString());
        
        // Get the full path
        boost::filesystem::path root(getenv("RAM_SVN_DIR"));
        boost::filesystem::path fullPath = root / shortPath;
        assert(boost::filesystem::exists(fullPath) &&
               "Nueral network file does not exists");
    
        // Load the network
        assert(m_net->create_from_file(fullPath.file_string()) &&
               "Nueral network file found, but error in loading");

        // Ensure it matches our parameters
        assert(getOutputCount() == (int)m_net->get_num_output() &&
               "Wrong network output count");
        assert(getNumberFeatures() == (int)m_net->get_num_input() &&
               "Wrong network input count");
    }
}
    
} // namespace vision
} // namespace ram

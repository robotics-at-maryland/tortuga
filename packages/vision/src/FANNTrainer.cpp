/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/src/ImageIdentifier.cpp
 */

// standard stuff
#include <iostream>
#include <sstream>
#include <math.h>

// for change_extension
#include <boost/filesystem/convenience.hpp>

// header for this class
#include "vision/include/FANNTrainer.h"
#include "vision/include/FANNSymbolDetector.h"

#define DATA_MIN 0.0
#define DATA_MAX 1.0
#define MIN_WEIGHT -0.0
#define MAX_WEIGHT 1.0
#define MIN_INIT_WEIGHT -0.8
#define MAX_INIT_WEIGHT 0.8

namespace ram {
namespace vision {

FANNTrainer::FANNTrainer(FANNSymbolDetectorPtr detector,
                         core::ConfigNode config) :
    m_fannDetector(detector)
{
    // the size of the input layer
    int outputCount = m_fannDetector->getOutputCount();
    const int inputSize = m_fannDetector->getNumberFeatures();
    
    // some config stuff
    m_cascade = config["UseCascade"].asInt(1);
    m_maxEpochs = config["MaxEpochs"].asInt(7500);
    m_sizeFactor = config["MaxSizeFactor"].asDouble(0.1);
    m_reportEpochs = config["ReportEpochs"].asInt(25);
    m_reportNeurons = config["ReportNeurons"].asInt(1);
    m_desiredError = config["DesiredError"].asDouble(0.0001);
    
    // an array of layer sizes - starting with input and ending with output
    int layers = config["Layers"].asInt(3);
    unsigned int layerSizes[layers];
    if (config["PyramidNetwork"].asInt(1))
    {
        // Make a pyramid of layers going from the input size, down to the
        // output size
        const int interval = (inputSize - outputCount) / (layers - 1);
        for (int i = 0; i < layers; ++i)
        {
            layerSizes[i] = inputSize - (interval * i);
        }

        // No matter what the above does, make sure first layer == inputSize
        // and last layer == outputCount
        layerSizes[0] = inputSize;
        layerSizes[layers - 1] = outputCount;
    }
    else
    {
        // Make all layers equal input size but the last
        for (int i = 0; i < layers; ++i)
        {
            layerSizes[i] = (i == layers - 1 ? outputCount : inputSize);
        }
    }
            
    // setup the network structure - if this doesn't work we're boned, hence
    // the assert
    if (m_cascade)
    {
        assert (m_net.create_shortcut_array (layers, layerSizes) &&
                "Failed to create neural network.\n");
    }
    else
    {
        assert (m_net.create_sparse_array(
                    config["ConnectionRate"].asDouble(0.75), layers,
                    layerSizes) && "Failed to create neural network.\n");
    }
            
    // set some parameters      
    if (m_cascade)
    {
        m_net.set_cascade_weight_multiplier (
            config["CascadeMultiplier"].asDouble(0.95));
        m_net.set_cascade_max_out_epochs (
            config["CascadeMaxOutEpochs"].asInt(250));
        m_net.set_cascade_output_change_fraction (
            config["CascadeOutputChange"].asDouble(0.20));
        m_net.set_cascade_candidate_change_fraction (
            config["CascadeCandidateChange"].asDouble(0.25));
        m_net.set_cascade_num_candidate_groups (
            config["CascadeGroups"].asInt(3));
        m_net.set_cascade_candidate_limit (
            config["CascadeCandidateLimit"].asInt(750));
    }
    m_net.set_learning_rate (config["LearningRate"].asDouble(0.35));
    m_net.set_learning_momentum(config["LearningMomentum"].asDouble(0.2));
    m_net.set_activation_steepness_hidden(
        config["InitialSteepness"].asDouble(0.5));
    m_net.set_activation_steepness_output(
        config["InitialSteepness"].asDouble(0.5));
    m_net.set_bit_fail_limit (config["BitFailLimit"].asDouble(0.10));
    m_net.set_activation_function_hidden (FANN::SIGMOID_STEPWISE);
    m_net.set_activation_function_output (FANN::SIGMOID_STEPWISE);
    m_net.set_train_error_function (FANN::ERRORFUNC_LINEAR);    
    m_net.set_train_stop_function (FANN::STOPFUNC_MSE);
    
    // generate some random starting weights
    m_net.randomize_weights(MIN_INIT_WEIGHT, MAX_INIT_WEIGHT);


    // Ensure the network is setup properly
    assert(m_fannDetector->getOutputCount() == (int)m_net.get_num_output() &&
           "Wrong network output count");
    assert(m_fannDetector->getNumberFeatures() == (int)m_net.get_num_input() &&
           "Wrong network input count");
}
                
/*
void FANNTrainer::loadFromFile (const boost::filesystem::path &file)
{
    boost::filesystem::path net = file;
    if (boost::filesystem::exists (net))
    {
        if (!m_net.create_from_file(net.file_string()))
        {
            std::cerr << "Error: network file '" << net
                      << "' exists but could not be read or loaded.\n";
        }
    } else
    {
        boost::filesystem::change_extension (net, "irn");
        if (boost::filesystem::exists (net))
        {
            if (!m_net.create_from_file(net.file_string()))
            {
                std::cerr << "Error: network file '" << net
                          << "' exists but could not be read or loaded.\n";
            }
        } else {
            std::cerr << "Error: network file '" << file
                      << "' does not exist.\n";
        }
    }
}
*/        
void FANNTrainer::runTraining (FANN::training_data &data)
{
    data.scale_train_data(DATA_MIN, DATA_MAX);
    m_net.reset_MSE();
    m_net.set_train_stop_function (FANN::STOPFUNC_MSE);
    m_net.set_training_algorithm (FANN::TRAIN_QUICKPROP);
    if (m_cascade)
    {
        m_net.cascadetrain_on_data(
            data,
            (unsigned int)m_sizeFactor * m_net.get_total_neurons(),
            m_reportNeurons, m_desiredError);
    }
    else
    {
        m_net.train_on_data (data, m_maxEpochs, m_reportEpochs, m_desiredError);
    }
    std::cout << "Training complete, MSE: " << m_net.get_MSE() << std::endl;
}
        
const void FANNTrainer::runTest (FANN::training_data &data,
                                     std::ostream &out)
{
    float MSE = 0.0;
    m_net.reset_MSE();
    m_net.test_data (data);
    out << "Test result: " << MSE << " MSE: " << m_net.get_MSE()
        << " Bit fail: " << m_net.get_bit_fail() << "\n";
}
    
const bool FANNTrainer::save (const boost::filesystem::path &file)
{
    bool value;
    boost::filesystem::path net = file;
    boost::filesystem::path training = file;
    if (boost::filesystem::extension (net) != ".irn")
    {
        net = boost::filesystem::path (net.file_string() + ".irn");
        value = m_net.save (net.file_string());
    }
    else
    {
        value = m_net.save (net.file_string());
    }
    return value;
}
        
bool FANNTrainer::addTrainData (unsigned int imageIndex,
                                FANN::training_data &data,
                                std::vector<Image*> &images,
                                bool debugOutput)
{
    if (images.size() == 0)
    {
        std::cerr << "ERROR: 0 images to train" << std::endl;
        return false;
    }
    if (imageIndex >= m_net.get_num_output())
    {
        std::cerr << "ERROR: imageIndex (" << imageIndex << ") >= "
                  << "network number of output (" << m_net.get_num_input()
                  << ")" << std::endl;
        return false;
    }
    
    // Load up all images int
    fann_type** input = new fann_type*[images.size()];
    fann_type** output = new fann_type*[images.size()];
    double* inputAverage = new double[m_net.get_num_input()];
    for (unsigned int j = 0; j < m_net.get_num_input(); ++j)
        inputAverage[j] = 0;
    for (unsigned int i = 0; i < images.size(); ++i)
    {
        // Fill in the input with our feature detector
        input[i] = new fann_type[m_net.get_num_input()];
        m_fannDetector->getImageFeatures(images[i], input[i]);
        
        // Fill in the output array based on the current desired output
        output[i] = new fann_type[m_net.get_num_output()];
        for (unsigned int y = 0; y < m_net.get_num_output(); ++y)
        {
            if (y == imageIndex)
                output[i][y] = DATA_MAX;
            else
                output[i][y] = DATA_MIN;
        }

        // Print the results of each input
        if (debugOutput)
        {
            for (unsigned int j = 0; j < m_net.get_num_input(); ++j)
            {
                std::cout << input[i][j] << " ";
                inputAverage[j] += input[i][j];
            }
            std::cout << "| ";
            for (unsigned int j = 0; j < m_net.get_num_output(); ++j)
                std::cout << output[i][j] << " ";
            std::cout << std::endl;
        }
    }

    if (debugOutput)
    {
        // Average the actualy input
        std::cout << "Training average: ";
        for (unsigned int j = 0; j < m_net.get_num_input(); ++j)
            std::cout << inputAverage[j] / images.size() << " ";
        std::cout << std::endl;
    }
    
    if (data.length_train_data() == 0)
    {
        // First training run
        data.set_train_data (images.size(), m_net.get_num_input(), input,
                             m_net.get_num_output(), output);
    }
    else
    {
        // Already trained before, so lets merge data
        FANN::training_data newData;
        newData.set_train_data (images.size(), m_net.get_num_input(), input,
                                m_net.get_num_output(), output);
        data.merge_train_data (newData);
    }
    
    // Clean up the input and output buffers
    for (unsigned int i = 0; i < images.size(); ++i)
    {
        delete[] input[i];
        delete[] output[i];
    }
    delete[] inputAverage;
    delete[] input;
    delete[] output;
    
    return true;
}    
    
} // namespace vision
} // namespace ram

/**
 *  imageNetwork.cpp
 *  fannTest
 *
 *  Created by David Love on 1/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 **/

#include <cassert>
#include <iostream>

// header for this file
#include "imageNetwork.hpp"

// defines to simplify stuff
#define INPUT_SIZE m_net.get_num_input()
#define OUTPUT_SIZE m_net.get_num_output()
#define TRAIN_SIZE m_data.length_train_data()

// consts and vars to be tweaked
#define CONNECTION_RATE 0.75
#define NUM_LAYERS 4
#define LEARNING_RATE 0.175
#define INITIAL_STEEPNESS 0.9
#define DATA_MIN -1.0
#define DATA_MAX 1.0
#define MAX_EPOCHS 1000
#define REPORT_EPOCHS 50
#define DESIRED_ERROR 0.125
#define MIN_WEIGHT -1.0
#define MAX_WEIGHT 1.0

using namespace FANN;

/**
 * Build an imageNetwork designd to handle a certian number of images of a given size. 
 **/
imageNetwork::imageNetwork (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidh) {
	// the size of the input layer
	const int inputSize = imageHeight * imageWidh;
	// an array of layer sizes - starting with input and ending with output
	unsigned int layerSizes[NUM_LAYERS];
	const int interval = (inputSize - images) / (NUM_LAYERS - 1);
	for (int i = 0; i < NUM_LAYERS; ++i) {
		layerSizes[i] = inputSize - (interval * i);
	}
	// setup the network structure - if this doesn't work we're boned
	assert (m_net.create_sparse_array (CONNECTION_RATE, NUM_LAYERS, layerSizes)); 
	// give the network a learning rate
	m_net.set_learning_rate (LEARNING_RATE);
	// set layer activation levels
    m_net.set_activation_steepness_hidden(INITIAL_STEEPNESS);
    m_net.set_activation_steepness_output(INITIAL_STEEPNESS);
    // set acivation functions for layers
    m_net.set_activation_function_hidden(SIGMOID_SYMMETRIC_STEPWISE);
    m_net.set_activation_function_output(SIGMOID_SYMMETRIC_STEPWISE);
	// generate some random starting weights
	m_net.randomize_weights(MIN_WEIGHT, MAX_WEIGHT);
}

/**
 * Reconsruct from a saved imageNetwork when given it's filename.
 **/
imageNetwork::imageNetwork (const std::string &file, bool loadTrainingData) {
	std::string netName;
	std::string trainName;
	// exact file name is given (either with or without the '.irn')
	if (m_net.create_from_file (file)) {
		if (loadTrainingData) {
			netName = file.substr (file.size() - 5, 4);
			if (netName == ".irn") {
				trainName = file.substr (0, file.size() - 4);
				trainName += ".trn";
			} else {
				trainName = file + ".trn";
			}
			if (!m_data.read_train_from_file(trainName)) {
				std::cerr << "Warning: training data file for network file '" << file << "' could not be read or loaded.";
			}
		}
	} else {
		// exact file name not given
		//     add '.irn' and try to open that, else there is an error
		netName = file + ".irn";
		if (m_net.create_from_file (netName)) {
			if (loadTrainingData) {
				trainName = file + ".trn";
				if (!m_data.read_train_from_file(trainName)) {
					std::cerr << "Warning: training data file for network file '" << file << "' could not be read or loaded.";
				}
			}
		} else {
			std::cerr << "Error: network file '" << file << "' could not be read or loaded.";
		}
	}
}

/**
 * Train an imageNetwork to recognize a given image based on an array of similar image data.
 **/
bool imageNetwork::addTrainData (unsigned int imageIndex, unsigned int images, const imageArray &imagesData) {
	if (imageIndex > OUTPUT_SIZE) {
		return false;
	}
	fann_type** inputData = (fann_type**) malloc (sizeof (fann_type) * images * INPUT_SIZE);
	fann_type** outputData = (fann_type**) malloc (sizeof (fann_type) * images * OUTPUT_SIZE);
	training_data newData = training_data ();
	// copy over the vector of image data into a nice single block of memory for the training_data object
	for (int i = 0; i < images; ++i) {
		for (int j = 0; j < INPUT_SIZE; ++j) {
			inputData[i][j] = imagesData[i][j];
		}
	}
	// construct an array to clamp output data to the correct values
	//     MAX for the node representing this image, MIN for the others
	for (int i = 0; i < images; ++i) {
		for (int j = 0; j < OUTPUT_SIZE; ++j) {
			if (j == imageIndex) {
				outputData[i][j] = DATA_MAX;
			} else {
				outputData[i][j] = DATA_MIN;
			}
		}
	}
	// create the new data set
	newData.set_train_data (images, images, inputData, images, outputData);
	// scale the new data to the max and min input/output values
	m_data.scale_input_train_data (DATA_MAX, DATA_MIN);
	m_data.scale_output_train_data (DATA_MAX, DATA_MIN);
	// add the new data o the old data
	m_data.merge_train_data(newData);
	free (inputData);
	free (outputData);
	return true;
}

/**
 * Train given the existing loaded training data.
 */
bool imageNetwork::runTraining () {
	// shuffle the training data so that it is given to the network in random order
	m_data.shuffle_train_data();
	m_net.train_on_data (m_data, MAX_EPOCHS, REPORT_EPOCHS, DESIRED_ERROR);
	return true;
}

/**
 * Save an imageNetwork such that it can be re-red and then either run or trained more.
 **/
bool imageNetwork::save (const std::string &file, bool saveTrainingData) {
	std::string netName;
	std::string trainName;
	if (file.substr (file.size() - 5, 4) == ".irn") {
		netName = file;
		trainName = file.substr (0, file.size() - 4) + ".trn";
	} else {
		netName = file + ".irn";
		trainName = file + ".trn";
	}
	if (m_net.save (netName)) {
		if (saveTrainingData) {
			if (!m_data.save_train (trainName)) {
				std::cerr << "Warning: training data for network: '" << netName << "' could not be saved to '" << trainName << "'";
			}
		}
	} else {
		std::cerr << "Error: network could not be saved to file '" << netName << "'";
		return false;
	}
	return true;
}

/**
 * Save an imageNetwork with a fixed point representation such that it will run efficently
 * but can longer be trained.
 **/
int imageNetwork::saveFinal (const std::string &file, bool saveTrainingData) {
	std::string netName;
	std::string trainName;
	int precision = 0;
	if (file.substr (file.size() - 5, 4) == ".irn") {
		netName = file;
		trainName = file.substr (0, file.size() - 4) + ".trn";
	} else {
		netName = file + ".irn";
		trainName = file + ".trn";
	}
	precision = m_net.save_to_fixed (netName);
	if (saveTrainingData) {
		if (!m_data.save_train_to_fixed (trainName, precision)) {
			std::cerr << "Warning: training data for network: '" << netName << "' could not be saved to '" << trainName << "'";
		}
	}
	return precision;
}

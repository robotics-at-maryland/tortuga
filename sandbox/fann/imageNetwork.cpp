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

// variables I created
#define MAX_SIZE_FACTOR 0.25
#define NUM_LAYERS 4
#define USE_CASCADE
// variables used by fann
#define CONNECTION_RATE 0.75
#define LEARNING_RATE 0.25
#define LEARNING_MOMENTUM 0.1
#define INITIAL_STEEPNESS 0.9
#define DATA_MIN -0.0
#define DATA_MAX 1.0
#define MAX_EPOCHS 6250
#define REPORT_EPOCHS 250
#define REPORT_NEURONS 1
#define DESIRED_ERROR 0.00001
#define MIN_WEIGHT -0.0
#define MAX_WEIGHT 1.0
#define MIN_INIT_WEIGHT -0.1
#define MAX_INIT_WEIGHT 0.1
#define CASCADE_MULTIPLIER 0.9
#define CASCADE_MAX_OUT_EPOCHS 250
#define BIT_FAIL_LIMIT 0.15
#define CASCADE_OUTPUT_CHANGE 0.01
#define CASCADE_CANDIDATE_CHANGE 0.01

using namespace FANN;

/**
 * Build an imageNetwork designd to handle a certian number of images of a given size. 
 **/
imageNetwork::imageNetwork (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidh) {
	// the size of the input layer
	const int inputSize = imageHeight * imageWidh;
	
#ifndef USE_CASCADE
	// an array of layer sizes - starting with input and ending with output
	unsigned int layerSizes[NUM_LAYERS];
	 const int interval = (inputSize - images) / (NUM_LAYERS - 1);
	 for (int i = 0; i < NUM_LAYERS; ++i) {
	 layerSizes[i] = inputSize - (interval * i);
	 }
#endif
	
	// setup the network structure - if this doesn't work we're boned, hence the assert
#ifdef USE_CASCADE
	assert (m_net.create_shortcut (2, inputSize, images));
#else
	assert (m_net.create_sparse_array (CONNECTION_RATE, NUM_LAYERS, layerSizes));
#endif
	
	// set some parameters
#ifdef USE_CASCADE
	m_net.set_cascade_weight_multiplier (CASCADE_MULTIPLIER);
	m_net.set_cascade_max_out_epochs (CASCADE_MAX_OUT_EPOCHS);
	m_net.set_cascade_output_change_fraction (CASCADE_OUTPUT_CHANGE);
	m_net.set_cascade_candidate_change_fraction (CASCADE_CANDIDATE_CHANGE);
	m_net.set_training_algorithm (TRAIN_QUICKPROP);
#else
	m_net.set_training_algorithm(TRAIN_BATCH);
#endif
	m_net.set_learning_rate (LEARNING_RATE);
	m_net.set_learning_momentum(LEARNING_MOMENTUM);
    m_net.set_activation_steepness_hidden(INITIAL_STEEPNESS);
    m_net.set_activation_steepness_output(INITIAL_STEEPNESS);
	m_net.set_bit_fail_limit (BIT_FAIL_LIMIT);
	m_net.set_activation_function_hidden (SIGMOID_STEPWISE);
	m_net.set_activation_function_output (SIGMOID_STEPWISE);
	m_net.set_train_error_function (ERRORFUNC_LINEAR);	
	m_net.set_train_stop_function (STOPFUNC_BIT);
	
	// generate some random starting weights
	m_net.randomize_weights(MIN_INIT_WEIGHT, MAX_INIT_WEIGHT);
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
			if (file.size() >=5 && file.substr (file.size() - 5, 4) == ".irn") {
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
	bool empty = (m_data.length_train_data() == 0);
	fann_type** inputData = (fann_type**) malloc (sizeof (fann_type*) * images);
	for (int i = 0; i < images; ++i) {
		inputData[i] = (fann_type*) malloc (sizeof (fann_type) * INPUT_SIZE);
	}
	fann_type** outputData = (fann_type**) malloc (sizeof (fann_type) * images);
	for (int i = 0; i < images; ++i) {
		outputData[i] = (fann_type*) malloc (sizeof (fann_type) * OUTPUT_SIZE);
	}
	training_data newData = training_data ();
	// copy over the vector of image data into a nice single block of memory for the training_data object
	// yes this is very inneficient, when I get a chance to look into how the create_train_from_callback works
	// I might be able to improve this signifigantly, but this way was quick and easy
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
	if (empty) {
		// create the new data set
		m_data.set_train_data (images, images, inputData, images, outputData);
		// scale the new data to the max and min input/output values
		m_data.scale_input_train_data (DATA_MIN, DATA_MAX);
		m_data.scale_output_train_data (DATA_MIN, DATA_MAX);
	} else {
		// create the new data set
		newData.set_train_data (images, images, inputData, images, outputData);
		// scale the new data to the max and min input/output values
		newData.scale_input_train_data (DATA_MIN, DATA_MAX);
		newData.scale_output_train_data (DATA_MIN, DATA_MAX);
		// add the new data o the old data
		m_data.merge_train_data(newData);
	}
	for (int i = 0; i < images; ++i) {
		free (inputData[i]);
		free (outputData[i]);
	}
	free (inputData);
	free (outputData);
	return true;
}

/**
 * Train given the existing loaded training data.
 */
bool imageNetwork::runTraining () {
	m_net.reset_MSE();
	// shuffle the training data so that it is given to the network in random order
	m_data.shuffle_train_data();
#ifdef USE_CASCADE
	m_net.cascadetrain_on_data (m_data, MAX_SIZE_FACTOR * m_net.get_total_neurons(), REPORT_NEURONS, DESIRED_ERROR);
	m_net.reset_MSE();
#endif
	m_net.train_on_data (m_data, MAX_EPOCHS, REPORT_EPOCHS, DESIRED_ERROR);
	return true;
}

/**
 * Run the network on a given input and use the largest output as the image determination.
 **/
int imageNetwork::run (std::vector<fann_type> inputs) {
	int highest_out = 0;
	fann_type* outputData;
	fann_type* inputData = (fann_type*) malloc (sizeof (fann_type) * INPUT_SIZE);
	for (int i = 0; i < INPUT_SIZE; ++i) {
		inputData[i] = inputs[i];
	}
	outputData = m_net.run (inputData);
	for (int i = 0; i < OUTPUT_SIZE; ++i) {
		if (outputData[i] > outputData[highest_out]) {
			highest_out = i;
		}
	}
	m_out = outputData[highest_out];
	free (inputData);
	free (outputData);
	return highest_out;
}

/**
 * Save an imageNetwork such that it can be re-red and then either run or trained more.
 **/
const bool imageNetwork::save (const std::string &file, bool saveTrainingData) {
	std::string netName;
	std::string trainName;
	if (file.size() >= 5 && (file.substr (file.size() - 5, 4) == ".irn")) {
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
const int imageNetwork::saveFinal (const std::string &file, bool saveTrainingData) {
	std::string netName;
	std::string trainName;
	int precision = 0;
	if (file.size() >= 5 && (file.substr (file.size() - 5, 4) == ".irn")) {
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

/*
 *  imageRecognizer.cpp
 *  fannTest
 *
 *  Created by David Love on 2/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

// standard stuff
#include <iostream>

// for BF::change_extension
#include <boost/filesystem/convenience.hpp>

// header for this class
#include "imageRecognizer.hpp"

// variables I created
#define MAX_SIZE_FACTOR 0.1
#define NUM_LAYERS 3

// network design parameters
#define USE_CASCADE
#define PYRAMID_NETWORK

// variables used by fann
#define CONNECTION_RATE 0.75
#define LEARNING_RATE 0.2
#define LEARNING_MOMENTUM 0.125
#define INITIAL_STEEPNESS 0.5
#define DATA_MIN 0.0
#define DATA_MAX 1.0
#define MAX_EPOCHS 6250
#define REPORT_EPOCHS 25
#define REPORT_NEURONS 1
#define DESIRED_ERROR 0.000001
#define MIN_WEIGHT -0.0
#define MAX_WEIGHT 1.0
#define MIN_INIT_WEIGHT -0.1
#define MAX_INIT_WEIGHT 0.1
#define CASCADE_MULTIPLIER 0.95
#define CASCADE_MAX_OUT_EPOCHS 250
#define BIT_FAIL_LIMIT 0.35
#define CASCADE_OUTPUT_CHANGE 0.1
#define CASCADE_CANDIDATE_CHANGE 0.25
#define CASCADE_GROUPS 5
#define CASCADE_CANDIDATE_LIMIT 750

// static members
std::vector<IplImage*> imageRecognizer::s_trainImages;
unsigned int imageRecognizer::s_trainIndex;

imageRecognizer::imageRecognizer (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidth): m_canTrain (true), m_maxIndex (0) {
	// the size of the input layer
	const int inputSize = imageHeight * imageWidth;

	// an array of layer sizes - starting with input and ending with output
	unsigned int layerSizes[NUM_LAYERS];
#ifdef PYRAMID_NETWORK
	const int interval = (inputSize - images) / (NUM_LAYERS - 1);
	for (int i = 0; i < NUM_LAYERS; ++i) {
		layerSizes[i] = inputSize - (interval * i);
	}
#else
	for (int i = 0; i < NUM_LAYERS; ++i) {
		layerSizes[i] = (i == NUM_LAYERS - 1 ? images : inputSize);
	}
#endif
	
	// setup the network structure - if this doesn't work we're boned, hence the assert
#ifdef USE_CASCADE
	assert (m_net.create_shortcut_array (NUM_LAYERS, layerSizes));
#else
	assert (m_net.create_sparse_array (CONNECTION_RATE, NUM_LAYERS, layerSizes));
#endif
	
	// set some parameters
#ifdef USE_CASCADE
	m_net.set_cascade_weight_multiplier (CASCADE_MULTIPLIER);
	m_net.set_cascade_max_out_epochs (CASCADE_MAX_OUT_EPOCHS);
	m_net.set_cascade_output_change_fraction (CASCADE_OUTPUT_CHANGE);
	m_net.set_cascade_candidate_change_fraction (CASCADE_CANDIDATE_CHANGE);
	m_net.set_cascade_num_candidate_groups (CASCADE_GROUPS);
	m_net.set_cascade_candidate_limit (CASCADE_CANDIDATE_LIMIT);
#endif
	m_net.set_learning_rate (LEARNING_RATE);
	m_net.set_learning_momentum(LEARNING_MOMENTUM);
    m_net.set_activation_steepness_hidden(INITIAL_STEEPNESS);
    m_net.set_activation_steepness_output(INITIAL_STEEPNESS);
	m_net.set_bit_fail_limit (BIT_FAIL_LIMIT);
	m_net.set_activation_function_hidden (FANN::SIGMOID_STEPWISE);
	m_net.set_activation_function_output (FANN::SIGMOID_STEPWISE);
	m_net.set_train_error_function (FANN::ERRORFUNC_LINEAR);	
	m_net.set_train_stop_function (FANN::STOPFUNC_MSE);
	
	// generate some random starting weights
	m_net.randomize_weights(MIN_INIT_WEIGHT, MAX_INIT_WEIGHT);
}

imageRecognizer::imageRecognizer (const BF::path &file, bool loadTrainingData): m_canTrain (false) {
	BF::path net = file;
	BF::path training = file;
	BF::change_extension (training, "trn");
	if (BF::exists (net)) {
		if (!m_net.create_from_file(net.file_string())) {
			std::cerr << "Error: network file '" << net << "' exists but could not be read or loaded.\n";
		}
	} else {
		BF::change_extension (net, "irn");
		if (BF::exists (net)) {
			if (!m_net.create_from_file(net.file_string())) {
				std::cerr << "Error: network file '" << net << "' exists but could not be read or loaded.\n";
			}
		} else {
			std::cerr << "Error: network file '" << file << "' does not exist.\n";
		}
	}
	if (loadTrainingData) {
		if (BF::exists (training)) {
			if (!m_data.read_train_from_file (training.file_string())) {
				std::cerr << "Warning: training data file '" << training << "' for network file '" << net << "' exists but could not be read or loaded.\n";
			}
		} else {
			std::cerr << "Warning: training data file '" << training << "' for network file '" << net << "' does not exist.\n";
		}
	}
	m_maxIndex = m_net.get_num_output();
}

int imageRecognizer::addTrainData (const std::vector<IplImage*> &images) {
	if (!m_canTrain) {
		return -1;
	}
	s_trainImages = images;
	s_trainIndex = m_maxIndex++;
	if (m_data.length_train_data() != 0) { 
		FANN::training_data newData;
		newData.create_train_from_callback (images.size(), m_net.get_num_input(), m_net.get_num_output(), &imageRecognizer::trainingCallback);
		m_data.merge_train_data(newData);
	} else {
		m_data.create_train_from_callback (images.size(), m_net.get_num_input(), m_net.get_num_output(), &imageRecognizer::trainingCallback);
	}
	return s_trainIndex;
}

void imageRecognizer::runTraining () {
	m_data.scale_train_data(DATA_MIN, DATA_MAX);
#ifdef USE_CASCADE
	m_net.reset_MSE();
	m_net.set_training_algorithm (FANN::TRAIN_QUICKPROP);
	m_net.cascadetrain_on_data (m_data, MAX_SIZE_FACTOR * m_net.get_total_neurons(), REPORT_NEURONS, DESIRED_ERROR);
#endif
	m_net.reset_MSE();
	m_net.set_training_algorithm (FANN::TRAIN_BATCH);
	m_net.train_on_data (m_data, MAX_EPOCHS, REPORT_EPOCHS, DESIRED_ERROR);
}

const void imageRecognizer::test () {
	float MSE = 0.0;
	m_net.reset_MSE();
	m_net.test_data (m_data);
	std::cout << "Test result: " << MSE << " MSE: " << m_net.get_MSE() << " Bit fail: " << m_net.get_bit_fail() << "\n";
}

int imageRecognizer::run (const IplImage* input) {
	int highest_out = 0;
	fann_type* outputData;
	IplImage* grayInput = cvCreateImage (cvSize(input->height, input->width), input->depth, 1);
	cvCvtColor (input, grayInput, CV_BGR2GRAY);
	fann_type* inputData = (fann_type*) malloc (sizeof (fann_type) * m_net.get_num_input());
	for (int i = 0; i < m_net.get_num_input(); ++i) {
		inputData[i] = (*(grayInput->imageData + (i * grayInput->widthStep)));
	}
	outputData = m_net.run (inputData);
	for (int i = 0; i < m_net.get_num_input(); ++i) {
		if (outputData[i] > outputData[highest_out]) {
			highest_out = i;
		}
	}
	m_outValue = outputData[highest_out];
	cvReleaseImage(&grayInput);
	free (inputData);
	free (outputData);
	return highest_out;
}

const bool imageRecognizer::save (const BF::path &file, bool saveTrainingData) {
	bool value;
	BF::path net = file;
	BF::path training = file;
	if (BF::extension (net) != "irn") {
		net = BF::path (net.file_string() + ".irn");
		value = m_net.save (net.file_string());
		if (saveTrainingData) {
			training = BF::path (training.file_string() + ".trn");
			value = m_data.save_train (training.file_string()) && value;
		}
	} else {
		value = m_net.save (net.file_string());
		if (saveTrainingData) {
			BF::change_extension (training, "trn");
			value = m_data.save_train (training.file_string()) && value;
		}
	}
	return value;
}

void imageRecognizer::trainingCallback (unsigned int setNum, unsigned int inputs, unsigned int outputs, fann_type* input, fann_type* output) {
	IplImage* img = cvCreateImage (cvSize (s_trainImages[setNum]->height, s_trainImages[setNum]->width), s_trainImages[setNum]->depth, 1);
	if (s_trainImages[setNum]->nChannels == 4) {
		cvCvtColor(s_trainImages[setNum], img, CV_BGR2GRAY);
	}
	for (int i = 0; i < inputs; ++i) {
		input[i] = (*(img->imageData + (i * img->widthStep)));
	}
	for (int i = 0; i < outputs; ++i) {
		if (i == s_trainIndex) {
			output[i] = DATA_MAX;
		} else {
			output[i] = DATA_MIN;
		}
	}
	cvReleaseImage (&img);
}

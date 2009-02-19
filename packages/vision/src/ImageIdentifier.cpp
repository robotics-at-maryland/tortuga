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

#include "vision/include/OpenCVImage.h"

// header for this class
#include "vision/include/ImageIdentifier.hpp"

// network design parameters
#define USE_CASCADE
#define PYRAMID_NETWORK

// variables I created
#define MAX_SIZE_FACTOR 0.1
#define NUM_LAYERS 3

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
#define DESIRED_ERROR 0.0001
#define MIN_WEIGHT -0.0
#define MAX_WEIGHT 1.0
#define MIN_INIT_WEIGHT -0.1
#define MAX_INIT_WEIGHT 0.1
#define BIT_FAIL_LIMIT 0.15

// cascade only fann variables
#define CASCADE_OUTPUT_CHANGE 0.1
#define CASCADE_CANDIDATE_CHANGE 0.25
#define CASCADE_GROUPS 5
#define CASCADE_CANDIDATE_LIMIT 750
#define CASCADE_MULTIPLIER 0.95
#define CASCADE_MAX_OUT_EPOCHS 250

namespace ram {
	namespace vision {
		ImageIdentifier::ImageIdentifier (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidth) {
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
			assert (m_net.create_shortcut_array (NUM_LAYERS, layerSizes) && "Failed to create neural network.\n");
#else
			assert (m_net.create_sparse_array (CONNECTION_RATE, NUM_LAYERS, layerSizes) && "Failed to create neural network.\n");
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
		
        ImageIdentifier::ImageIdentifier (const std::string &file) {
            loadFromFile (boost::filesystem::path (file));
        }
        
		ImageIdentifier::ImageIdentifier (const boost::filesystem::path &file) {
            loadFromFile (file);
		}
        
        void ImageIdentifier::loadFromFile (const boost::filesystem::path &file) {
			boost::filesystem::path net = file;
			if (boost::filesystem::exists (net)) {
				if (!m_net.create_from_file(net.file_string())) {
					std::cerr << "Error: network file '" << net << "' exists but could not be read or loaded.\n";
				}
			} else {
				boost::filesystem::change_extension (net, "irn");
				if (boost::filesystem::exists (net)) {
					if (!m_net.create_from_file(net.file_string())) {
						std::cerr << "Error: network file '" << net << "' exists but could not be read or loaded.\n";
					}
				} else {
					std::cerr << "Error: network file '" << file << "' does not exist.\n";
				}
			}
        }
		
		void ImageIdentifier::runTraining (FANN::training_data &data) {
			data.scale_train_data(DATA_MIN, DATA_MAX);
#ifdef USE_CASCADE
			m_net.reset_MSE();
			m_net.set_training_algorithm (FANN::TRAIN_QUICKPROP);	
			m_net.set_train_stop_function (FANN::STOPFUNC_BIT);
			m_net.cascadetrain_on_data (data, MAX_SIZE_FACTOR * m_net.get_total_neurons(), REPORT_NEURONS, DESIRED_ERROR);
#endif
			m_net.reset_MSE();
			m_net.set_training_algorithm (FANN::TRAIN_BATCH);	
			m_net.set_train_stop_function (FANN::STOPFUNC_MSE);
			m_net.train_on_data (data, MAX_EPOCHS, REPORT_EPOCHS, DESIRED_ERROR);
		}
		
		const void ImageIdentifier::runTest (FANN::training_data &data) {
			float MSE = 0.0;
			m_net.reset_MSE();
			m_net.test_data (data);
			std::cout << "Test result: " << MSE << " MSE: " << m_net.get_MSE() << " Bit fail: " << m_net.get_bit_fail() << "\n";
		}
		
		int ImageIdentifier::run (Image* input) {
            const unsigned int size = sqrt (m_net.get_num_input());
            std::cout << "Inputs: " << m_net.get_num_input() << " New Size: " << size << "!!!!!!!!!!!\n";
            print();
			unsigned int highest_out = 0;
            Image* resized = new OpenCVImage (size, size);
            resized->copyFrom (input);
            resized->setSize (size, size);
			IplImage* grayInput = grayscale (*resized);
			fann_type* inputData = new fann_type[m_net.get_num_input()];
			if (m_outValue) {
				free (m_outValue);
			}
			loadImage (grayInput, inputData);
			m_outValue = m_net.run (inputData);
			for (unsigned int i = 0; i < m_net.get_num_input(); ++i) {
				if (m_outValue[i] > m_outValue[highest_out]) {
					highest_out = i;
				}
			}
			cvReleaseImage(&grayInput);
            delete resized;
			delete inputData;
			return highest_out;
		}
		
		const bool ImageIdentifier::save (const boost::filesystem::path &file) {
			bool value;
			boost::filesystem::path net = file;
			boost::filesystem::path training = file;
			if (boost::filesystem::extension (net) != "irn") {
				net = boost::filesystem::path (net.file_string() + ".irn");
				value = m_net.save (net.file_string());
			} else {
				value = m_net.save (net.file_string());
			}
			return value;
		}
		
		bool ImageIdentifier::addTrainData (unsigned int imageIndex, FANN::training_data &data, const std::vector<Image*> &images) {
            const unsigned int size = sqrt (m_net.get_num_input());
			if (images.size() == 0 || imageIndex >= m_net.get_num_input()) {
				return false;
			}
            Image* resized = new OpenCVImage (size, size);
			IplImage* gray;
			fann_type** input = new fann_type*[images.size()];
			fann_type** output = new fann_type*[images.size()];
			for (unsigned int i = 0; i < images.size(); ++i) {
                resized->copyFrom (images[i]);
                resized->setSize (size, size);
				gray = grayscale (*resized);
				input[i] = new fann_type[m_net.get_num_input()];
				if (gray) {
					loadImage (gray, input[i]);
				}
				cvReleaseImage (&gray);
				output[i] = new fann_type[m_net.get_num_output()];
				for (unsigned int y = 0; y < m_net.get_num_output(); ++y) {
					if (y == imageIndex) {
						output[i][y] = DATA_MAX;
					} else {
						output[i][y] = DATA_MIN;
					}
				}
			}
			if (data.length_train_data() == 0) {
				data.set_train_data (images.size(), m_net.get_num_input(), input, m_net.get_num_output(), output);
			} else {
				FANN::training_data newData;
				newData.set_train_data (images.size(), m_net.get_num_input(), input, m_net.get_num_output(), output);
				data.merge_train_data (newData);
			}
			for (unsigned int i = 0; i < images.size(); ++i) {
				delete input[i];
				delete output[i];
			}
            delete resized;
			delete input;
			delete output;
			return true;
		}	
		
		void ImageIdentifier::loadImage (IplImage* src, fann_type* target) {
			unsigned int u = 0;
			for (int w = 0; w < src->width; ++w) {
				for (int h = 0; h < src->height; ++h) {
					target[u++] = getPixel (src, w, h);
				}
			}
			if (u < m_net.get_num_input()) {
				std::cout << "ImageIdentifier warning: image data is incomplete.\n";
				while (u < m_net.get_num_input()) {
					target[u++] = DATA_MIN;
				}
			}
		}
		
		IplImage* ImageIdentifier::grayscale (IplImage* src) {
			IplImage* dest = cvCreateImage (cvSize (src->width, src->height), IPL_DEPTH_8U, 1);
			if (src && dest && src->nChannels != 1) {
				cvCvtColor(src, dest, CV_BGR2GRAY);
			}
			return dest;
		}
	}
}

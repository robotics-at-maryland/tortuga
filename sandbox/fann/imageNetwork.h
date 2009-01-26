/*
 *  imageNetwork.h
 *  NNImageRecognition
 *
 *  Created by David Love on 1/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IMAGE_NETWORK_H
#define IMAGE_NETWORK_H

#include <string>

// for FANN
#include <floatfann.h>
#include <fann.h>
#include <fann_cpp.h>

using namespace FANN;

class imageNetwork {
private:
	neural_net m_net;
	training_data m_data;
public:
	imageNetwork (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidh);
	imageNetwork (const std::string &file, bool loadTrainingData);
	bool addTrainData (unsigned int imageindex, unsigned int images, float** imagesData);
	bool runTraining ();
	bool save (const std::string &file, bool saveTrainingData);
	int saveFinal (const std::string &file, bool saveTrainingData);
	static float* imgToInput (const unsigned char* image);
};

#endif

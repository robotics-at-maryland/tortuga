/*
 *  imageNetwork.h
 *  fannTest
 *
 *  Created by David Love on 1/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IMAGE_NETWORK_H
#define IMAGE_NETWORK_H

#include <string>
#include <vector>

typedef std::vector<std::vector<float> > imageArray;

// for FANN
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
	bool addTrainData (unsigned int imageindex, unsigned int images, const imageArray &imageData);
	bool runTraining ();
	bool save (const std::string &file, bool saveTrainingData);
	int saveFinal (const std::string &file, bool saveTrainingData);
};

#endif

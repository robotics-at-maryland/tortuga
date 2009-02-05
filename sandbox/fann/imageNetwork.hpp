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

// for FANN
#include <fann.h>
#include <fann_cpp.h>

using namespace FANN;

typedef std::vector<std::vector<fann_type> > imageArray;

class imageNetwork {
private:
	neural_net m_net;
	training_data m_data;
	fann_type m_out;
public:
	imageNetwork (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidth);
	imageNetwork (const std::string &file, bool loadTrainingData = true);
	bool addTrainData (unsigned int imageindex, unsigned int images, const imageArray &imageData);
	bool runTraining ();
	int run (std::vector<fann_type> inputs);
	inline const fann_type resultValue () { return m_out; }
	inline const void print () { m_net.print_parameters(); }
	const bool save (const std::string &file, bool saveTrainingData);
	const int saveFinal (const std::string &file, bool saveTrainingData);
};

#endif

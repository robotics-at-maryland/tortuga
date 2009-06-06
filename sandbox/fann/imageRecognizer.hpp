/*
 *  imageRecognizer.h
 *  fannTest
 *
 *  Created by David Love on 2/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 *  Just a note to anybody who may use this: this is very not thread safe while inside addTrainData()
 *  
 */

// standard stuff
#include <vector>

// image support
#include <opencv/cv.h>
#include <opencv/highgui.h>

// boost file stuff
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#define BF boost::filesystem

// fann library
#include <floatfann.h>
#include <fann_cpp.h>

class imageRecognizer {
private:
	FANN::neural_net m_net;
	FANN::training_data m_data;
	BF::path m_file;
	fann_type* m_outValue;
	const bool m_canTrain;
	unsigned int m_maxIndex;
	static std::vector<IplImage*> s_trainImages;
	static unsigned int s_trainIndex;
public:
	imageRecognizer (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidth);
	imageRecognizer (const BF::path &file, bool loadTrainingData = true);
	int addTrainData (const std::vector<IplImage*> &images);
	void runTraining ();
	int run (IplImage* input);
	const void runTest ();
	const bool save (const BF::path &file, bool saveTrainingData = true);
	inline const fann_type resultValue (unsigned int index) { return m_outValue[index]; }
	inline const void print () { m_net.print_parameters(); }
	static void trainingCallback (unsigned int setNum, unsigned int inputs, unsigned int outputs, fann_type* input, fann_type* output);
};
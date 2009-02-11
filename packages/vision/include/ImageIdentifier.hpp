/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/include/ImageIdentifier.hpp
 */

// standard stuff
#include <vector>

// boost file stuff
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

// fann library
#include <floatfann.h>
#include <fann_cpp.h>

// image manipulation
#include <opencv/cv.h>

// ram image header
#include "vision/include/Image.h"

namespace ram {
namespace vision {
    
class ImageIdentifier {
private:
	FANN::neural_net m_net;
	FANN::training_data m_data;
	boost::filesystem::path m_file;
	fann_type* m_outValue;
	const bool m_canTrain;
	unsigned int m_maxIndex;
	unsigned int m_trainIndex;
public:
	ImageIdentifier (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidth);
	ImageIdentifier (const boost::filesystem::path &file, bool loadTrainingData = true);
	int addTrainData (const std::vector<Image*> &images);
	void runTraining ();
	int run (Image* input);
	const void runTest ();
	const bool save (const boost::filesystem::path &file, bool saveTrainingData = true);
	inline const fann_type resultValue (unsigned int index) { return m_outValue[index]; }
	inline const void print () { m_net.print_parameters(); }
private:
	void loadImage (IplImage* src, fann_type* target);
	static IplImage* grayscale (IplImage* src);
	inline static const fann_type getPixel (IplImage* src, unsigned int w, unsigned int h) { return (src->imageData + (w * src->widthStep))[h]; }
};
    
} // namespace vision
} // namespace ram

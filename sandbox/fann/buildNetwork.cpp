/*
 *  build.cpp
 *  NNImageRecognition
 *
 *  Created by David Love on 1/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <string>
//#include <map>

// only new boost API's
#define BOOST_FILESYSTEM_NO_DEPRECATED
// boost filesystem libs
#include <boost/filesystem.hpp>
// because I'm lazy
#define bf boost::filesystem

// for images
#include <opencv/cv.h>
//#include <opencv/cv.hpp>
#include <opencv/highgui.h>

// neural network for differentiating images
#include "imageNetwork.h"

// utility function(s)
#include "cvToFloat.h"

// function prototypes
void usage();
unsigned int loadImages (const char* path, float** &imageData);

int main (int argc, char * const argv[]) {
	// check for enough args
	if (argc < 5) {
		usage();
		return 0;
	}
	// get the number of images and thier size
	const unsigned int images = atoi(argv[1]);
	const unsigned int height = atoi(argv[2]);
	const unsigned int width = atoi(argv[3]);
	// check to make sure there are enough images
	if (argc < images + 4) {
		usage();
		return 0;
	}
    // build the testing network
	// TODO: try to load from file first
	imageNetwork test = imageNetwork (images, height, width);
	// load each set of training images
	float** imagesData;
	unsigned int setSize;
	for (int image = 5; image < argc; ++image) {
		setSize = loadImages (argv[image], imagesData);
		test.addTrainData(image, images, imagesData);
		free (imagesData);
	}
	// save the network
	test.save (std::string (argv[4]), true);
	// done
    std::cout << "Yup\n";
    return 0;
}

// load a directory of images into a grayscale float array
unsigned int loadImages (const char* path, float** &imagesData) {
	int images = 0;
	float* image;
	bf::path directory (path);
	if (!bf::exists (directory)) {
		return 0;
	}
	bf::directory_iterator end;
	IplImage* img;
	for (bf::directory_iterator itr (directory); itr != end; ++itr) {
		img = 0;
		// 0 in the flags forces image to be loaded as grayscale
		img=cvLoadImage(itr->path().file_string().c_str(),0);
		if (img) {
			++images;
			imagesData = (float**) realloc (imagesData, sizeof (float**) * (images + 1) * img->height * img->width);
			image = cvToFloat(img);
			memcpy(imagesData[images],image, sizeof (float*) * img->height * img->width);
			free (image);
			++images;
			image = cvToFlippedFloat (img);
			memcpy(imagesData[images],image, sizeof (float*) * img->height * img->width);
			free (image);
			cvReleaseImage (&img);
		}
	}
	//TODO: finish this
	return images;
}

// print usage
void usage () {
	std::cout << "Usage:\n";
	std::cout << "buildNetwork images height width networkName trainingImages [trainingImages] ...\n";
	std::cout << "\timages:\tThe number of images the network will be trained to regocnize.\n";
	std::cout << "\theight:\tThe height in pixels of every training imaged and the images to\n\t\tbe recognized.\n";
	std::cout << "\twidth:\tThe width in pixels of every training image and the images to\n\t\tbe recognized.\n";
	std::cout << "\tnetworkName:\tThe file in which to store the network.  If this file\n\t\talready exists it will "
			  << "be opened and the network in it will be\n\t\ttrained further, if not it will be created and "
			  << "saved in this\n\t\tfile.\n";
	std::cout << "\ttrainingImages:\tA path to a set of images on which to train the network.\n\t\tThere should be "
			  << "one of these for each of the images to be\n\t\trecognized.  In other words, the number of these "
			  << "should be\n\t\tequal to the value of 'images'.\n";
}
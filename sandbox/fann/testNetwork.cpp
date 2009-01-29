/*
 *  testNetwork.cpp
 *  fannTest
 *
 *  Created by David Love on 1/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <vector>
#include <iostream>

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
#include "imageNetwork.hpp"
// utility function(s)
#include "imageSupport.hpp"

// function prototypes
void usage();

int main (int argc, char * const argv[]) {
	// check for enough args
	if (argc < 3) {
		usage();
		return 0;
	}
	std::vector<fann_type> image;
	int result;
	fann_type accuracy;
	// load the network
	imageNetwork test = imageNetwork (std::string (argv[1]));
	// load the image
	IplImage* img;
	img=cvLoadImage(argv[2],0);
	int size = img->width * img->height;
	for (int i = 0; i < size; ++i) {
		image.push_back (*(img->imageData + i * img->widthStep));
	}
	cvReleaseImage (&img);
	// test the image
	result = test.run(image);
	accuracy = test.resultValue();
	// output the results
	std::cout << "Result image index: " << result << " certainty: " << (accuracy * 100) << "\n";
	// done
    std::cout << "Yup\n";
    return 0;
}

// print usage
void usage () {
	std::cout << "Usage:\n";
	std::cout << "testNetwork networkName testImage\n";
	std::cout << "\tnetworkName:\tThe network file to load\n";
	std::cout << "\ttestImage:\tThe image with which to test the network\n";
}
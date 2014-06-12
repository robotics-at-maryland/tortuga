/*
 *  buildNetwork.cpp
 *  fannTest
 *
 *  Created by David Love on 1/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <map>
#include <string>
#include <iostream>

// only new boost API's
#define BOOST_FILESYSTEM_NO_DEPRECATED
// boost filesystem libs
#include <boost/filesystem.hpp>
// because I'm lazy
#define bf boost::filesystem

// neural network for differentiating images
#include "imageNetwork.hpp"
// utility function(s)
#include "imageSupport.hpp"

// function prototypes
void usage();

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
	// map a number to each imageDirectory
	std::map<int, imageDirectory> imageSets;
	// load each set of training images, printing the index for each path along the way
	for (int image = 5; image < argc; ++image) {
		imageSets.insert(std::make_pair (image - 5, imageDirectory (bf::path(argv[image]))));
		test.addTrainData(image - 5	, imageSets.find(image - 5)->second.size(), imageSets.find(image - 5)->second.getImages());
		test.addTrainData(image - 5	, imageSets.find(image - 5)->second.size(), imageSets.find(image - 5)->second.getImages(true));
		std::cout << "Directory: " << imageSets.find(image - 5)->second.path() << " assigned index: " << (image - 5) << "\n";
	}
	// print the original network
	test.print ();
	// train the network
	test.runTraining ();
	// print the trained network
	test.print ();
	// save the network
	test.save (std::string (argv[4]), true);
	// done
    std::cout << "Yup\n";
    return 0;
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
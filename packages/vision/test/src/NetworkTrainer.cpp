/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/test/src/NetworkTrainer.cpp
 */

#include <iostream>
#include <vector>

#include <boost/filesystem/convenience.hpp>

#include "vision/include/Image.h"
#include "vision/include/ImageIdentifier.hpp"

namespace rv = ram::vision;

namespace ram {
	namespace NetworkTrainer {
		// function prototypes
		void usage();
		std::vector<rv::Image*> loadDirectory (boost::filesystem::path &dir);
	}
}

using namespace ram::NetworkTrainer;

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
	if ((unsigned int) argc < images + 4) {
		usage();
		return 0;
	}
    // build the testing network
	// TODO: try to load from file first
	rv::ImageIdentifier test = rv::ImageIdentifier (images, height, width);
	std::vector<rv::Image*> imageList;
	boost::filesystem::path dirPath;
	int index;
	FANN::training_data data;
	// load each set of training images, printing the index for each path along the way
	for (int dir = 5; dir < argc; ++dir) {
		index = dir - 5;
		dirPath = boost::filesystem::path (argv[dir]);
        if (boost::filesystem::basename (dirPath) != "") {
		imageList = loadDirectory (dirPath);
		test.addTrainData (index, data, imageList);
		std::cout << "Directory: '" << argv[dir] << "' assigned index: " << index << "\n";
		for (unsigned int i = 0; i < imageList.size(); ++i) {
			delete imageList[i];
		}
		imageList.clear();
    }
	}
	// print the original network
	test.print ();
	// train the network
	std::cout << "Beginning training.\n";
	test.runTraining (data);
	// print the trained network
	test.print ();
	// test the network
	std::cout << "Running a network test (using existing training data).\n";
	test.runTest (data);
	// save the network
	dirPath = boost::filesystem::path (argv[4]);
	test.save (dirPath);
	// done
    std::cout << "Done\n";
    return 0;
}

std::vector<rv::Image*> ram::NetworkTrainer::loadDirectory (boost::filesystem::path &dir) {
	boost::filesystem::directory_iterator end;
	rv::Image* img = 0;
	std::vector<rv::Image*> imageList;
	if (!boost::filesystem::exists (dir)) {
		return imageList;
	}
	for (boost::filesystem::directory_iterator itr (dir); itr != end; ++itr) {
        if (boost::filesystem::basename (itr->path()) != "") {
		img = rv::Image::loadFromFile(itr->path().file_string());
		if (img) {
			// store the original image
			imageList.push_back (img);
		}
        }
	}
	return imageList;
}

// print usage
void ram::NetworkTrainer::usage () {
	std::cout << "Usage:\n";
	std::cout << "NetworkTrainer images height width networkName trainingImages [trainingImages] ...\n";
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

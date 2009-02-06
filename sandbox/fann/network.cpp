/*
 *  network.cpp
 *  fannTest
 *
 *  Created by David Love on 2/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>

#include "imageRecognizer.hpp"

// function prototypes
void usage();
std::vector<IplImage*> loadDirectory (BF::path &dir);
void unloadImages (std::vector<IplImage*> &images);

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
	imageRecognizer test = imageRecognizer (images, height, width);
	std::vector<IplImage*> imageList;
	std::vector<std::vector<IplImage*> > imageListList;
	BF::path dirPath;
	int index;
	// load each set of training images, printing the index for each path along the way
	for (int dir = 5; dir < argc; ++dir) {
		dirPath = BF::path (argv[dir]);
		imageList = loadDirectory (dirPath);
		imageListList.push_back(imageList);
		index = test.addTrainData (imageList);
		std::cout << "Directory: '" << argv[dir] << "' assigned index: " << index << "\n";
	}
	// print the original network
	test.print ();
	// train the network
	std::cout << "Beginning training.\n";
	test.runTraining ();
	// print the trained network
	test.print ();
	// test the network
	std::cout << "Running a network test.\n";
	test.runTest ();
	// save the network
	dirPath = BF::path (argv[4]);
	test.save (dirPath);
	// free some memory
	for (int i = 0; i < imageListList.size(); ++i) {
		unloadImages (imageListList[i]);
	}
	// done
    std::cout << "Yup\n";
    return 0;
}

std::vector<IplImage*> loadDirectory (BF::path &dir) {
	int images = 0;
	IplImage* img;
	BF::directory_iterator end;
	std::vector<IplImage*> imageList;
	if (!BF::exists (dir)) {
		return imageList;
	}
	for (BF::directory_iterator itr (dir); itr != end; ++itr) {
		img = 0;
		// 0 in the flags forces image to be loaded as grayscale
		img = cvLoadImage(itr->path().file_string().c_str(),0);
		if (img) {
			// store the original image
			imageList.push_back (img);
			++images;
		}
	}
	return imageList;
}

void unloadImages (std::vector<IplImage*> &images) {
	for (int i = 0; i < images.size(); ++i) {
		cvReleaseImage(&(images[i]));
	}
	images.clear();
}

// print usage
void usage () {
	std::cout << "Usage:\n";
	std::cout << "network images height width networkName trainingImages [trainingImages] ...\n";
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
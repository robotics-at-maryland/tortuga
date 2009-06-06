/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love <loved@umd.edu>
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/include/ImageIdentifier.hpp
 */

#ifndef RAM_VISION_IMAGE_IDENTIFIER_HPP
#define RAM_VISION_IMAGE_IDENTIFIER_HPP

// standard stuff
#include <vector>
#include <iostream>

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
#include "core/include/ConfigNode.h"

// thingus
#include "vision/include/Export.h"

namespace ram {
	namespace vision {

		class RAM_EXPORT ImageIdentifier {
		private:
			FANN::neural_net m_net;
			fann_type* m_outValue;
            bool m_cascade;
            float m_sizeFactor, m_desiredError;
            int m_maxEpochs, m_reportEpochs, m_reportNeurons;
		public:
			/** Build a new ImageIdentifier **/
			ImageIdentifier (const unsigned int images, const unsigned int imageHeight, const unsigned int imageWidth, core::ConfigNode config = ram::core::ConfigNode::fromString("{}"));
			/** Reconstruct an ImageIdentifier from a saved file **/
			ImageIdentifier (const std::string &file);
			/** Reconstruct an ImageIdentifier from a saved file **/
			ImageIdentifier (const boost::filesystem::path &file);
			/** Train on a given set of data **/
			void runTraining (FANN::training_data &data);
			/** Run the ImageIdentifier on a given input image **/
			int run (Image* input);
			/** Test the ImageIdentifier with a set of training data **/
			const void runTest (FANN::training_data &data, std::ostream &out = std::cout);
			/** Save this ImageIdentifier to a file **/
			const bool save (const boost::filesystem::path &file);
			/** Get the value of a given output neuron **/
			inline const fann_type resultValue (unsigned int index) { return m_outValue[index]; }
			/** Print a representation of this ImageIdentifier's fann network internals **/
			inline const void print () { m_net.print_parameters(); }
			/** Add a set of images to a set fo training data for use with this ImageIdentifier **/
			bool addTrainData (unsigned int imageIndex, FANN::training_data &data, const std::vector<Image*> &images);
            /** Make an image grayscale **/
			static IplImage* grayscale (IplImage* src);
		private:
            /** Load this network from a file **/
            void loadFromFile (const boost::filesystem::path &file);
            /** Load an image's data into an array of fann_types **/
			void loadImage (IplImage* src, fann_type* target);
            /** Get a single pixel form an image (assumes it's a grayscale image) **/
			inline static const fann_type getPixel (IplImage* src, unsigned int w, unsigned int h) { return (src->imageData + (w * src->widthStep))[h]; }
		};
		
	} // namespace vision
} // namespace ram

#endif

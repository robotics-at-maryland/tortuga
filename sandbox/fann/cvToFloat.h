/*
 *  cvToFloat.h
 *  NNImageRecognition
 *
 *  Created by David Love on 1/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CV_TO_FLOAT_H
#define CV_TO_FLOAT_H

// for images
#include <opencv/cv.h>
//#include <opencv/cv.hpp>
#include <opencv/highgui.h>

// get an array of float representing the stripped down grayscale image
float* cvToFloat (IplImage* img) {
	int size = img->width * img->height;
	float* values = new float[size];
	for (int i = 0; i < size; ++i) {
		values[i] = *(img->imageData + i * img->widthStep);
	}
	return values;
}

// get the grayscaled float array of an image, but upside down
float* cvToFlippedFloat (IplImage* img) {
	IplImage* flippedImg;
	float* flippedFloat;
	cvConvertImage(img, flippedImg, CV_CVTIMG_FLIP);
	flippedFloat = cvToFloat(flippedImg);
	cvReleaseImage (&flippedImg);
	return flippedFloat;
}

#endif

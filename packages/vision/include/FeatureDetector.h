/*
 *  RedLightDetector.h
 *  
 *
 *  Created by Daniel Hakim on 12/4/07.
 *  Copyright 2007 Daniel Hakim. All rights reserved.
 *
 */

#ifndef RAM_FEATURE_DETECTOR_H_06_23_2007
#define RAM_FEATURE_DETECTOR_H_06_23_2007

#include <iostream>
#include <sstream>
#include <math.h>
#include <cstdlib>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <string>
#include "vision/include/main.h"
#include "vision/include/ProcessList.h"
#include "vision/include/VisionCommunication.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Image.h"
#include "vision/include/Camera.h"
#include "vision/include/Export.h"

namespace ram { namespace vision {
	class RAM_EXPORT FeatureDetector
	{
		public:
			FeatureDetector(OpenCVCamera*, int=1000);
			~FeatureDetector();
			void update();
			void show(char* window);
			IplImage* getAnalyzedImage();
			int maxFeatures;
		private:
			IplImage* image;
			IplImage* grayscale;
			IplImage* raw;
			IplImage* eigImage;
			IplImage* tempImage;
			IplImage* edgeDetected;
			CvPoint2D32f* features;
			
			void copyChannel(IplImage*, IplImage*, int);
			
			ram::vision::Image* frame;
			ram::vision::OpenCVCamera* cam;
			
	};	
}}//ram::vision

#endif // RAM_FEATURE_DETECTOR_H_06_23_2007

/*
 *  RedLightDetector.h
 *  
 *
 *  Created by Daniel Hakim on 6/22/07.
 *  Copyright 2007 Daniel Hakim. All rights reserved.
 *
 */

#ifndef RAM_RED_LIGHT_DETECTOR_H_06_23_2007
#define RAM_RED_LIGHT_DETECTOR_H_06_23_2007

#include <iostream>
#include <sstream>
#include <math.h>
#include <cstdlib>
#include <unistd.h>
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

namespace ram { namespace vision {
	class RedLightDetector
	{
		public:
			bool found;
			RedLightDetector(OpenCVCamera*);
			~RedLightDetector();
			void update();

		private:
		int lightFramesOff;
		int lightFramesOn;
		int blinks;
		int spooky;
		bool startCounting;
		CvPoint lightCenter;
		
	
		ram::vision::Image* frame;
		ram::vision::Camera* cam;
	};
	
}}//ram::vision

#endif // RAM_RED_LIGHT_DETECTOR_H_06_23_2007

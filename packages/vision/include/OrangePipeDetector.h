/*
 *  OrangePipeDetector.h
 *  
 *
 *  Created by Daniel Hakim on 6/22/07.
 *  Copyright 2007 Daniel Hakim. All rights reserved.
 *
 */

#ifndef RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007
#define RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007

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
	class OrangePipeDetector
	{
		public:
			bool found;
			OrangePipeDetector(OpenCVCamera*);
			~OrangePipeDetector();
			//get normalized center of line.
			double getX();
			double getY();
			double getAngle();
			void update();

		private:
		bool show_output;
		double angle;
		double lineX;
		double lineY;
		
		ram::vision::Image* frame;
		ram::vision::Camera* cam;
	};
	
}}//ram::vision

#endif // RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007


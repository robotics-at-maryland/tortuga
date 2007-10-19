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
	class RAM_EXPORT OrangePipeDetector
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
			void show(char* window);
			IplImage* getAnalyzedImage();


		private:
		double angle;
		double lineX;
		double lineY;
		IplImage* rotated;
		ram::vision::Image* frame;
		ram::vision::OpenCVCamera* cam;
	};
	
}}//ram::vision

#endif // RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007


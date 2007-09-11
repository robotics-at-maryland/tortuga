/*
 *  GateDetector.h
 *  
 *
 *  Created by Daniel Hakim on 6/22/07.
 *  Copyright 2007 Daniel Hakim. All rights reserved.
 *
 */
 
#ifndef RAM_GATE_DETECTOR_H_06_23_2007
#define RAM_GATE_DETECTOR_H_06_23_2007

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
	class GateDetector
	{
		public:
			bool found;
			GateDetector(ram::vision::OpenCVCamera*);
			~GateDetector();
			void update();
			double getX();
			double getY();
			void show(char* window);
			IplImage* getAnalyzedImage();

		private:
			bool show_output;
			int gateX;
			int gateY;
			double gateXNorm;
			double gateYNorm;
			IplImage* gateFrame;
			IplImage* gateFrameRatios;
	
			ram::vision::Image* frame;
			ram::vision::OpenCVCamera* cam;
	};
	
}}//ram::vision

#endif // RAM_GATE_DETECTOR_H_06_23_2007

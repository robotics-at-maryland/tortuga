/*
 *  BinDetector.h
 *  
 *
 *  Created by Daniel Hakim on 6/22/07.
 *  Copyright 2007 Daniel Hakim. All rights reserved.
 *
 */

#ifndef RAM_RECORDER_H_06_24_2007
#define RAM_RECORDER_H_06_24_2007

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
	class Recorder
	{
		public:
			Recorder(ram::vision::OpenCVCamera*, const char* filename);
			~Recorder();
			void update();
			void writeFrame(IplImage* img);
			CvVideoWriter *writer;
			int isColor;
			int fps;  // or 30
			int frameW; // 744 for firewire cameras
			int frameH; // 480 for firewire cameras
		private:
			ram::vision::Image* frame;
			ram::vision::Camera* cam;
	};
	
}}//ram::vision

#endif // RAM_RECORDER_H_06_24_2007

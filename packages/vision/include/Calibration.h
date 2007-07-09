/*
 *  Calibration.h
 *  
 *
 *  Created by Daniel Hakim on 6/22/07.
 *  Copyright 2007 Daniel Hakim. All rights reserved.
 *
 */

#ifndef RAM_CALIBRATION_H_07_05_2007
#define RAM_CALIBRATION_H_07_05_2007

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

#define NUMIMAGES_CALIBRATE 50

namespace ram{
namespace vision{
	class Calibration
	{
		public:
			Calibration(ram::vision::OpenCVCamera* camera);
			~Calibration();
			void calculateCalibrations();
			void printCalibrations();
			void calibrateImage(IplImage* src, IplImage* dest);
			void setCalibrationManual(float* distortion, float* cameraMatrix, float* rotMat, float* transVects);
            void setCalibration(bool forward);

		private:
			float distortion[4];
			float cameraMatrix[9];
			ram::vision::Image* frame;
			ram::vision::Camera* cam;
			bool calibrated;
			float transVects[3*NUMIMAGES_CALIBRATE];//should be 3
			float rotMat[9*NUMIMAGES_CALIBRATE];//should be 9
			IplImage* dest;
	};
}
}

#endif // RAM_RAM_CALIBRATION_H_07_05_2007

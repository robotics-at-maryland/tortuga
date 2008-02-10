/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/Calibration.h
 */

#ifndef RAM_CALIBRATION_H_07_05_2007
#define RAM_CALIBRATION_H_07_05_2007

#define _CRT_SECURE_NO_WARNINGS

// Project Includes
#include "vision/include/Common.h"

// Must be included last
#include "vision/include/Export.h"

#define NUMIMAGES_CALIBRATE 50

namespace ram{
namespace vision{
	class RAM_EXPORT Calibration
	{
		public:
			Calibration(Camera* camera);
			~Calibration();
			void calculateCalibrations();
			void printCalibrations();
			void calibrateImage(IplImage* src, IplImage* dest);
			void setCalibrationManual(float* distortion, float* cameraMatrix, float* rotMat, float* transVects);
            void setCalibration(bool forward);

		private:
			float distortion[4];
			float cameraMatrix[9];
			Image* frame;
			Camera* cam;
			bool calibrated;
			float transVects[3*NUMIMAGES_CALIBRATE];//should be 3
			float rotMat[9*NUMIMAGES_CALIBRATE];//should be 9
			IplImage* dest;
	};
} // namespace vision
} // namespace ram

#endif // RAM_RAM_CALIBRATION_H_07_05_2007

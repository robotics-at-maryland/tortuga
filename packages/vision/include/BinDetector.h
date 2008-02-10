/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/BinDetector.h
 */

#ifndef RAM_BIN_DETECTOR_H_06_23_2007
#define RAM_BIN_DETECTOR_H_06_23_2007

// Project Includes
#include "vision/include/Common.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
	class RAM_EXPORT BinDetector
	{
		public:
			bool found;
			BinDetector(Camera*);
			~BinDetector();
			void update();
			void show(char* window);
			double getX();
			double getY();
			IplImage* getAnalyzedImage();

		private:
			double binX;
			double binY;
			int binCount;
			IplImage* binFrame;
			IplImage* rotated;
		Image* frame;
		Camera* cam;
	};
	
} // namespace vision
} // namespace ram

#endif // RAM_BIN_DETECTOR_H_06_23_2007

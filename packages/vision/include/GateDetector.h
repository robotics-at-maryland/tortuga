/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/GateDetector.h
 */

#ifndef RAM_GATE_DETECTOR_H_06_23_2007
#define RAM_GATE_DETECTOR_H_06_23_2007

// Project Includes
#include "vision/include/Common.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
	class RAM_EXPORT GateDetector
	{
		public:
			bool found;
			GateDetector(Camera*);
			~GateDetector();
			void update();
			double getX();
			double getY();
			void show(char* window);
			IplImage* getAnalyzedImage();

		private:
			int gateX;
			int gateY;
			double gateXNorm;
			double gateYNorm;
			IplImage* gateFrame;
			IplImage* gateFrameRatios;
	
			Image* frame;
			Camera* cam;
	};
	
} // namespace vision
} // namespace ram

#endif // RAM_GATE_DETECTOR_H_06_23_2007

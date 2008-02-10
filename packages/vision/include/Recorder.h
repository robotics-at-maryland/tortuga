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

#define _CRT_SECURE_NO_WARNINGS

// Project Includes
#include "vision/include/Common.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
	class RAM_EXPORT Recorder
	{
		public:
			Recorder(Camera*, const char* filename);
			~Recorder();
			void update();
			void writeFrame(IplImage* img);
			CvVideoWriter *writer;
			int isColor;
			int fps;  // or 30
			int frameW; // 744 for firewire cameras
			int frameH; // 480 for firewire cameras
		private:
			Image* frame;
			Camera* cam;
	};
} // namespace vision
} // namespace ram	

#endif // RAM_RECORDER_H_06_24_2007

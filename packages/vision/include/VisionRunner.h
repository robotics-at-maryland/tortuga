/*
 *  VisionRunner.h
 *  
 *
 *  Created by Daniel Hakim on 7/11/07.
 *  Copyright 2007 Daniel Hakim. All rights reserved.
 *
 */
#ifndef RAM_VISION_RUNNER_H_07_11_2007
#define RAM_VISION_RUNNER_H_07_11_2007

#include "vision/include/DetectorTest.h"
#include "vision/include/Export.h"
#include "vision/include/Events.h"
#include "core/include/EventPublisher.h"

namespace ram{ namespace vision{
class VisionRunner{
	public:
		VisionRunner();
		void captureEvent(ram::core::EventPtr e);
		DetectorTest forward; 
		DetectorTest downward;
};
}}
#endif

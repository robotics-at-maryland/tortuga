#include "vision/include/DetectorTest.h"
#include "vision/include/VisionRunner.h"
#include <highgui.h>
#include <iostream>
#include <signal.h>
#include <string>
namespace ram{
namespace vision
{
	VisionRunner::VisionRunner():
		forward(300,true),
		downward(301,false)
	{
		forward.background(25);
		downward.background(25);
	}
	
	void dataCopy(unsigned char* a, int b, int c)
	{
		printf("%s","WHY IS THIS BEING CALLED?  TURN OFF DEMO IN DETECTORTEST IF YOURE GOING TO RUN WITH VISION RUNNER!!\n");
		//Just here to make the compiling happy, this is only needed for VisionDemo
	}
}
}


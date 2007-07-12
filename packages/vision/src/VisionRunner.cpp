#include "vision/include/DetectorTest.h"
#include "vision/include/VisionRunner.h"
#include <highgui.h>
#include <iostream>
namespace ram{
namespace vision
{
	VisionRunner::VisionRunner():
		forward(300,true),
		downward(301,false)
	{
		forward.background();
		downward.background();
	}
}
}

int main()
{
	ram::vision::DetectorTest forward(0,true);
//	forward.background(30);
	forward.update(5);
	while (true)
	{
		char key=cvWaitKey();
		if (key =='q')
		{
			forward.unbackground(true);
			return 0;
		}
		else if (key == '1')
		{
			if (forward.orangeOn)
				forward.orangeDetectOff();
			else
				forward.orangeDetectOn();
		}
		else if (key == '2')
		{
			if (forward.lightOn)
				forward.lightDetectOff();
			else
				forward.lightDetectOn();
		}
		else if (key == '3')
		{
			if (forward.binOn)
				forward.binDetectOff();
			else
				forward.binDetectOn();
		}
		else if (key == '4')
		{
			if (forward.gateOn)
				forward.gateDetectOff();
			else
				forward.gateDetectOn();
		}
		else
			std::cout<<"Welcome to the new detector tester, type keys 1 through 4 to enable/disable filters, and type q to quit. \n 1: Orange, 2: Light, 3: Bin, 4: Gate, q: Quit\n\n";
	}
}

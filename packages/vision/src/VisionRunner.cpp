#include "vision/include/DetectorTest.h"
#include "vision/include/VisionRunner.h"
#include <highgui.h>
#include <iostream>
#include <signal.h>
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

using namespace std;
char key=' '; //cvWaitKey(25);

void handler(int x)
{
  key='q';
}

int main()
{
  //	usleep(10000000);
	ram::vision::DetectorTest forward(0,true);
	forward.background(30);
	forward.orangeDetectOn();
	forward.lightDetectOn();
	forward.binDetectOn();
	forward.gateDetectOn();
	signal(SIGINT,handler);
	while (true)
	{

		if (key =='q')
		{
			forward.unbackground(true);
			return 0;
		}
		else if (key == '1')
		{
			if (forward.orangeOn)
			{
				forward.orangeDetectOff();
			}
			else
			{
				std::cout<<"ORANGE ON"<<endl;
				forward.orangeDetectOn();
			}
		}
		else if (key == '2')
		{
			if (forward.lightOn)
			{
				forward.lightDetectOff();
			}
			else
			{
				forward.lightDetectOn();
				std::cout<<"LIGHT ON"<<endl;

			}
		}
		else if (key == '3')
		{
			if (forward.binOn)
			{
				forward.binDetectOff();
			}
			else
			{
				forward.binDetectOn();
				std::cout<<"BIN ON"<<endl;
			}
		}
		else if (key == '4')
		{
			if (forward.gateOn)
				forward.gateDetectOff();
			else
			{
				forward.gateDetectOn();
				std::cout<<"Gate On"<<endl;
			}
		}
//		else
//			std::cout<<"Welcome to the new detector tester, type keys 1 through 4 to enable/disable filters, and type q to quit. \n 1: Orange, 2: Light, 3: Bin, 4: Gate, q: Quit\n\n";
	}
}

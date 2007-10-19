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

char key=' '; //cvWaitKey(25);

using namespace std;

void handler(int x)
{
  key='q';
}


int main(int argc, char** argv)
{
	ram::vision::DetectorTest* forward;
	if (argc==2)
	{
		string moviename=argv[1];
		cout<<moviename<<endl;
		forward=new ram::vision::DetectorTest(moviename);
	}
	else
		forward=new ram::vision::DetectorTest(0,true);
//	forward->orangeDetectOn();
//	forward->lightDetectOn();
//	forward->binDetectOn();
//	forward->gateDetectOn();
	signal(SIGINT,handler);
	while (true)
	{
		key=(char)cvWaitKey(50);
		forward->update(0);
		if (key =='q')
		{
			std::cout<<"Quitting Normally"<<endl;
			forward->unbackground(true);
			delete forward;
			return 0;
		}
		else if (key == '1')
		{
			if (forward->orangeOn)
			{
				forward->orangeDetectOff();
			}
			else
			{
				std::cout<<"ORANGE ON"<<endl;
				forward->orangeDetectOn();
			}
		}
		else if (key == '2')
		{
			if (forward->lightOn)
			{
				forward->lightDetectOff();
			}
			else
			{
				forward->lightDetectOn();
				std::cout<<"LIGHT ON"<<endl;

			}
		}
		else if (key == '3')
		{
			if (forward->binOn)
			{
				forward->binDetectOff();
			}
			else
			{
				forward->binDetectOn();
				std::cout<<"BIN ON"<<endl;
			}
		}
		else if (key == '4')
		{
			if (forward->gateOn)
				forward->gateDetectOff();
			else
			{
				forward->gateDetectOn();
				std::cout<<"Gate On"<<endl;
			}
		}
	}
}

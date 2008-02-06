#include "vision/include/DetectorTest.h"
#include <highgui.h>
#include <iostream>
#include <signal.h>
#include <string>

char key=' '; //cvWaitKey(25);

void handler(int x)
{
  key='q';
}

void myImageEventHandler(ram::core::EventPtr e)
{
	boost::shared_ptr<ram::vision::ImageEvent> imageEvent = 
	    boost::dynamic_pointer_cast<ram::vision::ImageEvent>(e);
	printf("%f %f", imageEvent->x,imageEvent->y);
}

void myPipeEventHandler(ram::core::EventPtr e)
{
	boost::shared_ptr<ram::vision::PipeEvent> pipeEvent = 
	    boost::dynamic_pointer_cast<ram::vision::PipeEvent>(e);
	printf("\n\nPIPE! X: %f Y: %f ANGLE: %f \n\n", pipeEvent->x, pipeEvent->y, pipeEvent->angle);
}

using namespace std;
//To run this program with a movie, send it command line arguments
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
  //forward->orangeDetectOn();
  forward->lightDetectOn();
  //forward->binDetectOn();
  //forward->gateDetectOn();
  signal(SIGINT,handler);
//  forward->subscribe(ram::vision::EventType::LIGHT_FOUND, myImageEventHandler);
//  forward->subscribe(ram::vision::EventType::PIPE_FOUND,myPipeEventHandler);
  while (true)
    {
      key=(char)cvWaitKey(50);
      forward->update(0);
      if (key =='q')
	{
//	  std::cout<<"Quitting Normally"<<endl;
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
      else if (key == '5')
	{
	  if (forward->featureOn)
	    forward->featureDetectOff();
	  else
	    {
	      forward->featureDetectOn();
	      std::cout<<"Feature Detection On"<<endl;
	    }
	}
    }
}

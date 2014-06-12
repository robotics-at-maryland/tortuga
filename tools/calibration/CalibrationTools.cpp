#include <highgui.h>

//Most of these are completely unnecessary... I don't care at the moment.
#include "vision/include/DetectorTest.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/GateDetector.h"
#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Calibration.h"
#include <string.h>
#include <iostream>

using namespace std;
using namespace ram::vision;

int main(int argc,char** argv)
{
  OpenCVCamera* forward=NULL;
  OpenCVCamera* downward=NULL;
  bool forwardOn=false;
  bool downwardOn=false;
  if (argc==2)
    {
      if (strncmp(argv[1],"forward",8)==0)
	{
	  forward  = new OpenCVCamera(300,true);
	  forwardOn=true;
	}
      else
	{
	  forward  = new OpenCVCamera(300,true);
	  downward = new OpenCVCamera(301,false);
	  downwardOn=true;
	}
    }
  else
    {
      forwardOn=true;
      downwardOn=true;
    }
  forward->background();
  downward->background();
  
  Calibration* downwardCal= new Calibration(downward);
  Calibration* forwardCal=new Calibration(forward);  
  
  if (downwardOn)
    {
      printf("Printing Downward Calibrations");
      downwardCal->printCalibrations();
    }
  else if (forwardOn)
    {
      printf("Printing Forward Calibrations");
      forwardCal->printCalibrations();
    }
}

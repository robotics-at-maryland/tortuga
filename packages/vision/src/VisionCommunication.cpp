/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/src/VisionCommunication.cpp
 */

// STD Includes
#include <stdlib.h>
#include <stdio.h>
// Project Includes
#include "vision/include/VisionCommunication.h"

namespace ram {
namespace vision {

  extern "C"{
    VisionCommunication* vc=(VisionCommunication*) malloc(sizeof(VisionCommunication));
  }

  VisionCommunication* getCommunicator()
  {
	  return vc;
  }
VisionData* getDummy()
{
  vc->dummyCheck.frameNum=2;
  vc->dummyCheck.width=3;
  vc->dummyCheck.height=5;
  vc->dummyCheck.redLightx=7;
  vc->dummyCheck.redLighty=11;
  vc->dummyCheck.distFromVertical=13;
  vc->dummyCheck.angle=17;
  vc->dummyCheck.binx=19;
  vc->dummyCheck.biny=23;
  vc->dummyCheck.lightVisible=29;
  vc->dummyCheck.pipeVisible=31;
  vc->dummyCheck.binVisible=37;
  vc->dummyCheck.frameNumCheck=41;
  
  return &(vc->dummyCheck);
}

VisionData* getUnsafe()
{
  return &(vc->unsafe);
}
 
VisionData** getSafe()
{
  return (vc->safe);
}

VisionData* getData()
{
  if (vc->safe==NULL)
    return NULL;
  return captureSnapshot(10);
}

VisionData* captureSnapshot(int tries)
{
  int x=0;
  for(x=0;x<tries;x++)
    {
      VisionData* buffer=*(vc->safe);
      vc->guaranteed.frameNum=(buffer->frameNum);
      vc->guaranteed.width=(buffer->width);
      vc->guaranteed.height=(buffer->height);
      vc->guaranteed.redLightx=(buffer->redLightx);
      vc->guaranteed.redLighty=(buffer->redLighty);
      vc->guaranteed.distFromVertical=(buffer->distFromVertical);
      vc->guaranteed.angle=(buffer->angle);
      vc->guaranteed.binx=(buffer->binx);
      vc->guaranteed.biny=(buffer->biny);
      vc->guaranteed.lightVisible=(buffer->lightVisible);
      vc->guaranteed.pipeVisible=(buffer->pipeVisible);
      vc->guaranteed.binVisible=(buffer->binVisible);
      vc->guaranteed.frameNumCheck=(buffer->binVisible);
      if (vc->guaranteed.frameNum==vc->guaranteed.frameNumCheck)
	return &(vc->guaranteed);
    }
  return NULL;
}	

} // namespace vision
} // namespace ram

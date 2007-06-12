#include <stdlib.h>
#include "VisionData.h"
#include "vision/include/VisionCommunication.cpp"

VisionData* VisionCommunication::getDummy()
{
  dummyCheck.frameNum=2;
  dummyCheck.width=3;
  dummyCheck.height=5;
  dummyCheck.redLightx=7;
  dummyCheck.redLighty=11;
  dummyCheck.distFromVertical=13;
  dummyCheck.angle=17;
  dummyCheck.binx=19;
  dummyCheck.biny=23;
  dummyCheck.lightVisible=29;
  dummyCheck.pipeVisible=31;
  dummyCheck.binVisible=37;
  dummyCheck.frameNumCheck=41;
  
  return &dummyCheck;
}

VisionData* VisionCommunication::getUnsafe()
{
  return &unsafe;
}
 
VisionData** VisionCommunication::getSafe()
{
  return safe;
}

VisionData* VisionCommunication::getData()
{
  return captureSnapshot(10);
}

VisionData* VisionCommunication::captureSnapshot(int tries)
{
  int x=0;
  for(x=0;x<tries;x++)
    {
      VisionData* buffer=*safe;
      guaranteed.frameNum=(buffer->frameNum);
      guaranteed.width=(buffer->width);
      guaranteed.height=(buffer->height);
      guaranteed.redLightx=(buffer->redLightx);
      guaranteed.redLighty=(buffer->redLighty);
      guaranteed.distFromVertical=(buffer->distFromVertical);
      guaranteed.angle=(buffer->angle);
      guaranteed.binx=(buffer->binx);
      guaranteed.biny=(buffer->biny);
      guaranteed.lightVisible=(buffer->lightVisible);
      guaranteed.pipeVisible=(buffer->pipeVisible);
      guaranteed.binVisible=(buffer->binVisible);
      guaranteed.frameNumCheck=(buffer->binVisible);
      if (guaranteed.frameNum==guaranteed.frameNumCheck)
	return &guaranteed;
    }
  return NULL;
}	

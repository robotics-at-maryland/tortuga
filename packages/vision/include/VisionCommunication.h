/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/include/VisionCommunication.h
 */

#ifndef RAM_VISION_VISIONCOMMUNICATION_H_06_11_2007
#define RAM_VISION_VISIONCOMMUNICATION_H_06_11_2007

#include "vision/include/VisionData.h"
#include "vision/include/Export.h"

namespace ram {
namespace vision {

struct RAM_EXPORT VisionCommunication
{
// public:
  VisionData unsafe;
  VisionData** safe;
  VisionData guaranteed;
  VisionData dummyCheck;
  
};

 extern "C"
  {
    VisionData* getDummy();
    VisionData* getUnsafe();
    VisionData** getSafe();
    VisionData* getData();
    VisionData* captureSnapshot(int tries);
	VisionCommunication* getCommunicator();
  }

} // namespace vision
} // namespae ram

#endif // RAM_VISION_VISIONCOMMUNICATION_H_06_11_2007

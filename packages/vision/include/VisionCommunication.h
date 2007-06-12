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

namespace ram {
namespace vision {

class VisionCommunication
{
 public:
  VisionData unsafe;
  VisionData** safe;
  VisionData guaranteed;
  VisionData dummyCheck;
  
  VisionData* getDummy();
  VisionData* getUnsafe();
  VisionData** getSafe();
  VisionData* getData();
 private:
  VisionData* captureSnapshot(int tries);
};

} // namespace vision
} // namespae ram

#endif // RAM_VISION_VISIONCOMMUNICATION_H_06_11_2007

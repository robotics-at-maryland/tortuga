/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Camera.h
 */

#ifndef RAM_VISION_CAMERA_H_05_23_2007
#define RAM_VISION_CAMERA_H_05_23_2007

// Vision Includes
#include "vision/include/Common.h"

namespace ram {
namespace vision {

class Camera
{
public:
    virtual Image* getImage() = 0;
};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_CAMERA_H_05_23_2007

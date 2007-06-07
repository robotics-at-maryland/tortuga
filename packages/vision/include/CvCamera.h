/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/include/CvCamera.h
 */

#ifndef RAM_VISION_CVCAMERA_H_06_06_2007
#define RAM_VISION_CVCAMERA_H_06_06_2007

// Vision Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"

struct CvCapture;

namespace ram {
namespace vision {

class CvCamera : public Camera
{
public:
    CvCamera(int camNum =0);
    ~CvCamera();

    Image* getImage();

private:
	CvCapture* camCapture;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_CAMERA_H_06_06_2007

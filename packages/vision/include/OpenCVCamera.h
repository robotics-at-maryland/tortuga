/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim and Joseph Lisee
 * File:  packages/vision/include/OpenCVCamera.h
 */

#ifndef RAM_VISION_OPENCVCAMERA_H_06_06_2007
#define RAM_VISION_OPENCVCAMERA_H_06_06_2007

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"

// Forward Declartions
struct CvCapture; // For OpenCV

namespace ram {
namespace vision {

class OpenCVCamera : public Camera
{
public:
    OpenCVCamera(int camNum =0);
    ~OpenCVCamera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);

    virtual size_t width();
    
    virtual size_t height();

private:
    CvCapture* m_camCapture;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVCAMERA_H_06_06_2007

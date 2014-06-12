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
#include <string>

// Library Includes
#include <cv.h>
#include <highgui.h>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"
#include "vision/include/Export.h"

// Forward Declartions
struct CvCapture; // For OpenCV

namespace ram {
namespace vision {

class RAM_EXPORT OpenCVCamera : public Camera
{
public:
    /** Open specific camera */
    OpenCVCamera(int camNum = 0);

    /** Open a specific movie */
    OpenCVCamera(std::string movieName);
    
    virtual ~OpenCVCamera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);

    virtual size_t width();
    
    virtual size_t height();

    virtual double fps();

    virtual double duration();

    virtual void seekToTime(double seconds);

    virtual double currentTime();
    
private:    
    /** OpenCV Capture handle */
    cv::VideoCapture* m_camCapture;
    cv::Mat m_mat;
    IplImage m_buffer;

    bool m_live;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVCAMERA_H_06_06_2007

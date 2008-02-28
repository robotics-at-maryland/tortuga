/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/ImageCamera.h
 */

#ifndef RAM_VISION_IMAGECAMERA_H_02_23_2008
#define RAM_VISION_IMAGECAMERA_H_02_23_2008

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/Camera.h"
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** A minmal camera class which can be driven by external images */
class RAM_EXPORT ImageCamera : public ram::vision::Camera
{
public:
    ImageCamera(size_t width, size_t height, size_t fps);

    virtual ~ImageCamera();

    /** Calls captured image with the given image */
    void newImage(ram::vision::Image* image);
    
    /** Not valid with this camera */
    virtual void update(double timestep);
    
    virtual size_t width();

    virtual size_t height();

    virtual size_t fps();
    
    virtual bool backgrounded();

    virtual void background(int);

    virtual void unbackground(bool join = false);

protected:
    bool _backgrounded;
    size_t m_fps;
    size_t m_width;
    size_t m_height;
};

}  // namespace vision
}  // namespace ram

#endif // RAM_VISION_IMAGECAMERA_H_02_23_2008

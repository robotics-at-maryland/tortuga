/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/MockCamera.h
 */

#ifndef RAM_VISION_TEST_MOCKCAMERA_H_02_07_2008
#define RAM_VISION_TEST_MOCKCAMERA_H_02_07_2008

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/Camera.h"

class MockCamera : public ram::vision::Camera
{
public:
    /** Create a mock camera using the given images
     *
     *  @param newImage
     *       The image passed to capturedImage.
     *
     *  @param publicImage
     *       The image copied to Public Image during copyToPublic.  If 0, then
     *       newImage is used.
     */
    MockCamera(ram::vision::Image* newImage_ = 0,
               ram::vision::Image* publicImage_ = 0);

    virtual ~MockCamera();
    
    void setNewImage(ram::vision::Image* newImage);

    void setPublicImage(ram::vision::Image* publicImage);

    void setImages(ram::vision::Image* newImage,
                   ram::vision::Image* publicImage);
    
    /** Calls capturedImage with newImage as the argument */
    virtual void update(double timestep);
    
    virtual size_t width();

    virtual size_t height();

    virtual bool backgrounded()
        { return _backgrounded; }
    
    virtual void background(int)
        { _backgrounded = true; }

    virtual void unbackground(bool join = false)
    {
        _backgrounded = false;
        Camera::unbackground(join);
    };


protected:
    bool _backgrounded;
    
    ram::vision::Image* m_newImage;
    
    ram::vision::Image* m_publicImage;
    
    /** Copies the internal publicImage to supplied publicImage
     *
     *  If the internal publicImage is 0, then newImage is copied to publicImage
     *  instead.
     */
    virtual void copyToPublic(ram::vision::Image* newImage,
                              ram::vision::Image* publicImage);
};

#endif // RAM_VISION_TEST_MOCKCAMERA_H_02_07_2008

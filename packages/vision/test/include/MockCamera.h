/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/MockCamera.h
 */

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
    MockCamera(ram::vision::Image* newImage_,
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

protected:
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

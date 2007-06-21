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

// Project Includes
#include "pattern/include/Subject.h"
#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"

#include "vision/include/Common.h"

namespace ram {
namespace vision {

/** The base class for all Cameras.
 *
 *  The only thing needed is to implement the update function and save the
 *  internal image to to m_privateImage;
 */
class Camera : public pattern::Subject, public core::Updatable
{
public:
    /** The flags passed to observers */
    enum ObserverFlags {
        IMAGE_CAPTURED
    };

    Camera();
    virtual ~Camera();
    
    /** Retrieves the latest image from the camera
     *
     *  @current  The current image is copied into the given image
     *            and that pointer is returned.
     */
    virtual void getImage(Image* current);

    /** Grabs an image from the camera and saves it to the internal buffer*/
    virtual void update(double timestep) = 0;

    /** Width of captured image */
    virtual size_t width() = 0;

    /** Height of captured image */
    virtual size_t height() = 0;

    /** Returns the current FPS of the camera */
    //virtual size_t fps() = 0;
    
protected:
    /** Notifies all observers that an image has been captured
     *
     * @param newImage  This image is copied to the public image in a thread
     *                  safe manner. The public image is access by getImage.
     */
    virtual void capturedImage(Image* newImage);
    
private:
    /** Protects access to the public image */
    core::ReadWriteMutex m_imageMutex;

    /** Image returned from get image*/
    Image* m_publicImage;
};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_CAMERA_H_05_23_2007

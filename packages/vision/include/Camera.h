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
#include "core/include/CountDownLatch.h"
//#include "core/include/Event.h"
#include "core/include/EventPublisher.h"

#include "vision/include/Common.h"
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** The base class for all Cameras.
 *
 *  When implementing a new camera, all you have to do is overload the virtual
 *  functions below.  When overloading update, call capturedImage
 *
 */
class RAM_EXPORT Camera : public core::Updatable,
                          public core::EventPublisher
{
public:
    /** The image sent to capturedImage, before any processing
     *
     *  <b>DO NOT</b> do a lot of work in these event handlers.  It will block
     *  the camera capture thread.  The best usage is just to copy the given
     *  image to an internal buffer or queue.
     */
    static const core::Event::EventType RAW_IMAGE_CAPTURED;
    
    /** The image available through getImage
     *
     *  <b>DO NOT</b> do a lot of work in these event handlers.  It will block
     *  the camera capture thread.  The best usage is just to copy the given
     *  image to an internal buffer or queue.
     */
    static const core::Event::EventType IMAGE_CAPTURED;
    
    virtual ~Camera();
    
    /** Retrieves the latest image from the camera.
     *
     *  @current  The current image is copied into the given image
     *            and that pointer is returned.
     */
    void getImage(Image* current);

    /** Retrieves the next image from the camera.
     *
     *  This will block until the next image is grabed from the camera then call
     *  getImage.
     *
     *  @current  The current image is copied into the given image
     *            and that pointer is returned.
     */
    void waitForImage(Image* current);
    
    /** Grabs an image from the camera and saves it with capturedImage
     *
     *  This is where the work of the Camera is done.  Just get the new image
     *  and call capturedImage.  This is run from a background thread, when you
     *  background on the Camera.
     *
     */
    virtual void update(double timestep) = 0;

    /** Width of captured image */
    virtual size_t width() = 0;

    /** Height of captured image */
    virtual size_t height() = 0;

    /** Returns the current FPS of the camera */
    //virtual size_t fps() = 0;
    
protected:
    /** No argument constructor
     *
     *  You can not give the Camera and ram::core::EventHub to pass events on
     *  too because we wish to control the access to the image pointers.
     */
    Camera();
    
    /** This must be called in the destructor of all subclasses
     *
     * @warning  This has to be called <b>BEFORE</b> releasing any object
     *           related to image capture.  If this is not done the background
     *           capture thread will still try and grab images.
     */
    void cleanup();
    
    /** Copies image to the public images release those waiting on the image
     *
     *  This use the virtual copyToPublicImage function to perform the copy. If
     *  you wish to optimize the copy or perform some kind of processing, that
     *  is the function you should overload.
     *
     * @param newImage  This image is copied to the public image in a thread
     *                  safe manner. The public image is accessed by getImage.
     */
    void capturedImage(Image* newImage);

    /** Copies the newImage to publich image.
     *
     *  Override this if you wish to do some kind of post processing on the
     *  image such as rotation or undistortion.
     */
    virtual void copyToPublic(Image* newImage, Image* publicImage);
    
private:
    /** Protects access to the public image */
    core::ReadWriteMutex m_imageMutex;

    /** Image returned from get image*/
    Image* m_publicImage;
    
    /** Latch to release threads waiting on a new image */
    core::CountDownLatch m_imageLatch;
    
    /** Recoreds whether or not the cleanup */
    bool m_cleanedUp;
};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_CAMERA_H_05_23_2007

/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee
 * File:  packages/vision/include/DC1394Camera.h
 */

#ifndef RAM_VISION_DC1394CAMERA_H_02_20_2009
#define RAM_VISION_DC1394CAMERA_H_02_20_2009

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"
#include "vision/include/Export.h"

#include "core/include/ConfigNode.h"

// Forward Declartions
struct __dc1394_camera; 
typedef struct __dc1394_camera dc1394camera_t;
struct __dc1394_t;
typedef struct __dc1394_t dc1394_t;
struct __dc1394_video_frame;
typedef struct __dc1394_video_frame dc1394video_frame_t;


namespace ram {
namespace vision {

/** Captures images from firewire cameras using the libdc1394 library */
class RAM_EXPORT DC1394Camera : public Camera
{
public:
    /** Opens the first firewire camera found on the bus */
    DC1394Camera(core::ConfigNode config);

    DC1394Camera(core::ConfigNode config, size_t num);
    
    /** Open specific camera by its GUI */
    DC1394Camera(core::ConfigNode config, uint64_t guid);

    /** Shuts down the camera */
    virtual ~DC1394Camera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);
	
    virtual size_t width();
    
    virtual size_t height();

    virtual double fps();

    virtual double duration();

    virtual void seekToTime(double seconds);

    virtual double currentTime();

    ///TODO: Overload background so that we flush the buffer of images before
    // we start caputuring
    
private:
    /** Init with the camera with the given GUID */
    void init(core::ConfigNode config, uint64_t guid);

    /** Sets the brightness to the desired value (or auto) */
    void setBrightness(uint32_t value, bool makeAuto = false);

    /** Sets the exposure to the desired value (or auto) */
    void setExposure(uint32_t value, bool makeAuto = false);

    /** Sets teh brightness to the desired value (or auto) */
    void setWhiteBalance(uint32_t uValue, uint32_t vValue,
                         bool makeAuto = false);

    /** Balance the white values */
    void balanceWhite();
    
    /** Initializes libdc1394 if needed
     *
     *  Initializes libdc1394 if this is the first DC1394 camera created.
     */
    static void initLibDC1394();

    /** Shutsdown libdc1394 if needed
     *
     *  Only shutsdown libdc1394 if this is the last 1394 library.
     */
    static void shutdownLibDC1394();
    
    /** Calculated from the video mode chosen for the camera */
    uint32_t m_width;
    
    /** Calculated from the video mode chosen for the camera */
    uint32_t m_height;
    
    /** Calculated from the underlying chosen framerate */
    double m_fps;
    
    /** libdc1394 camera structure */
    dc1394camera_t* m_camera;

    /** Lastest image grabbed from the camera */
    dc1394video_frame_t* m_newFrame;
    
    /** Handle for the library */
    static dc1394_t* s_libHandle;
    
    /** The number of cameras around, a reference count for s_libHandle */
    static size_t s_camCount;

    /** Use own custom white balance */
    bool m_customWhiteBalance;
    
    /** Whether or not the whitebalance is supported by the camera */
    bool m_hasWhiteBalance;

    /** Maximum white balance value */
    unsigned int m_whiteMax;

    /** Minimum white balance value */
    unsigned int m_whiteMin;

    /** U or Blue value of the white balance*/
    unsigned int m_uValue;

    /** V or Red value of the white balance */
    unsigned int m_vValue;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_DC1394CAMERA_H_02_20_2009

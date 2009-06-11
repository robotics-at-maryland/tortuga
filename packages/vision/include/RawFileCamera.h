/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/RawFileCamera.h
 */

#ifndef RAM_VISION_RAWFILECAMERA_H_06_11_2009
#define RAM_VISION_RAWFILECAMERA_H_06_11_2009

// Project Includes
#include "vision/include/Camera.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** Captures images from firewire cameras using the libdc1394 library */
class RAM_EXPORT RawFileCamera : public Camera
{
public:
    /** Opens the given file with libavformat and decodes with libavcodec */
    RawFileCamera(std::string fileName);

    /** Shuts down the camera */
    virtual ~RawFileCamera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);
	
    virtual size_t width();
    
    virtual size_t height();

    virtual double fps();

    virtual double duration();

    virtual void seekToTime(double seconds);

    virtual double currentTime();

    /**
     * Jumps to a specific frame.
     * @param frame The frame to jump to.
     */
    void seekTo(int frame);
    
private:
    /** Read the next frame from the video file
     *
     *  @param hurryUp
     *      Don't decode frames for display, just advance through the file
     */
    void readNextFrame(bool hurryUp = false);
    
    /** Calculated from the video mode chosen for the camera */
    size_t m_width;
    
    /** Calculated from the video mode chosen for the camera */
    size_t m_height;
    
    /** Calculated from the underlying chosen framerate */
    double m_fps;

    /** Length of file */
    double m_duration;

    /** Current place in the file */
    double m_currentTime;

    /** Our current frame number */
    int m_currentFrame;

    /** Holds the raw picture data read from the file */
    unsigned char* m_dataBuffer;

    /** Size of the raw data buffer */
    size_t m_dataBufferSize;

    int m_file;

    /** The size of the underlying file */
    int m_fileSize;
    //FILE* m_file;
};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_RAWFILECAMERA_H_06_11_2009

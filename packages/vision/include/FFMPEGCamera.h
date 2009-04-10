/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 John Edmonds
 * All rights reserved.
 *
 * Author: John "Jack" Edmonds <pocketcookies2@gmail.com>
 * File:  packages/vision/include/FFMPEGCamera.h
 */

#ifndef RAM_VISION_FFMPEGCAMERA_H_11_17_2008
#define RAM_VISION_FFMPEGCAMERA_H_11_17_2008

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

// Project Includes
#include "vision/include/Camera.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** Captures images from firewire cameras using the libdc1394 library */
class RAM_EXPORT FFMPEGCamera : public Camera
{
public:
    /** Opens the given file with libavformat and decodes with libavcodec */
    FFMPEGCamera(std::string fileName);

    /** Shuts down the camera */
    virtual ~FFMPEGCamera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);
	
    virtual size_t width();
    
    virtual size_t height();

    virtual double fps();

    virtual double duration();

    virtual void seekToTime(double seconds);

    virtual double currentTime();

    /**
     * Changes the current frame by frames many frames.
     * @param frames The number of frames to seek by.
     */
    void seekBy(int frames);
    /**
     * Jumps to a specific frame.
     * @param frame The frame to jump to.
     */
    void seekTo(int frame);
    
private:
    /** Read the next frame from the video file */
    void readNextFrame();
    
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

    /** Timebase used to convert timestamp and frames */
    double m_timeBase;

    /** The index of the video stream in our container */
    int m_videoStreamIndex;
    
    /** Context for opening the video file */
    AVFormatContext* m_formatContext;

    /** Context for decoding the video stream */
    AVCodecContext* m_codecContext;

    /** The raw frame returned from the decoder */
    AVFrame* m_frame;
    
    /** The converted frame */
    AVFrame* m_RGBframe;
    
    /** The buffer used to the store the converted data */
    uint8_t* m_pictureBuffer;

    int m_currentFrame;
    
    struct SwsContext* m_convertContext;
    
};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_FFMPEGCAMERA_H_11_17_2008

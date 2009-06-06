/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FFMPEGNetworkCamera.h
 */

#ifndef RAM_VISION_FFMPEGNETWORKCAMERA_H_07_30_2008
#define RAM_VISION_FFMPEGNETWORKCAMERA_H_07_30_2008

// Project Includes
#include "vision/include/NetworkCamera.h"
#include "vision/include/Export.h"

extern "C" {
struct AVCodecContext;
struct AVFrame;
struct SwsContext;
}

namespace ram {
namespace vision {
    
class RAM_EXPORT FFMPEGNetworkCamera : public NetworkCamera
{
public:
    /** Creates a camera which read dad from the given host the given port */
    FFMPEGNetworkCamera(std::string hostname, boost::uint16_t port);
    
    ~FFMPEGNetworkCamera();

protected:
    /** Decompresses the incoming buffer */
    virtual void decompress(unsigned char* compressedBuffer,
                            size_t compressedSize,
                            unsigned char* outputBuffer);
    
private:
    AVFrame* allocPicture(int pix_fmt, int width, int height);
    
    /** Holds state information for libavcodec and encoding */
    AVCodecContext* m_codecContext;
    /** Context for image format conversion */
    struct SwsContext* m_convertContext;
    /** Holds the image in the codec native format */
    AVFrame* m_picture;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_FFMPEGNETWORKCAMERA_H_07_30_2008

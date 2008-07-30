/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FFMPEGnetworkRecorder.h
 */

#ifndef RAM_FFMPEGNETWORKRECORDER_H_07_29_2008
#define RAM_FFMPEGNETWORKRECORDER_H_07_29_2008

// Project Includes
#include "vision/include/NetworkRecorder.h"

// Must be included last
#include "vision/include/Export.h"

extern "C" {
struct AVCodecContext;
typedef struct AVCodecContext;
struct AVFrame;
typedef struct AVFrame;
struct SwsContext;
}

namespace ram {
namespace vision {

/** Sends data from the given camera over the ffmpegnetwork */
class RAM_EXPORT FFMPEGNetworkRecorder : public NetworkRecorder
{
  public:
    /** Creates a record which broadcasts data using TCP on the given port */
    FFMPEGNetworkRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                          boost::uint16_t port, int policyArg = 0);

    virtual ~FFMPEGNetworkRecorder();

  protected:
    /** Compresses the given data into the given output buffer
     *
     * @return The number of bytes of the output buffer used
     */
    virtual size_t compress(Image* toCompress, unsigned char* output,
                            size_t outputSize);
    
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

#endif // RAM_NETWORKRECORDER_H_07_29_2008

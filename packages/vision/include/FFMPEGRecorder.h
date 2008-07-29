/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FFMPEGRecorder.h
 */

#ifndef RAM_FFMPEGRECORDER_H_02_24_2008
#define RAM_FFMPEGRECORDER_H_02_24_2008

// Project Includes
#include "vision/include/Recorder.h"

// Must be included last
#include "vision/include/Export.h"

extern "C" {
struct AVFormatContext;
typedef struct AVFormatContext;
struct AVStream;
typedef struct AVStream;
struct AVOutputFormat;
typedef struct AVOutputFormat;
struct AVFrame;
typedef struct AVFrame;
struct SwsContext;
}

namespace ram {
namespace vision {

class RAM_EXPORT FFMPEGRecorder : public Recorder
{
  public:
    FFMPEGRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                   std::string filename, int policyArg = 0);

    virtual ~FFMPEGRecorder();

    virtual void background(int) {};
    virtual void unbackground(bool) {};
    
  protected:
    /** Called whenever there is a frame to record, records to disk */
    virtual void recordFrame(Image* image);
    
  private:
    /** Add video to the stream */
    AVStream* addVideoStream(int codec_id_);
    /** Open Video codecs allocates buffers */
    void openVideo();
    /** Allocates and AV frame */
    static AVFrame* allocPicture(int pix_fmt, int width, int height);
    
    /** Holds state information for libavformat about our output video */
    AVFormatContext* m_formatContext;
    /** Holds state information about the video stream we are writing */
    AVStream* m_videoStream;
    /** The output format we are using (ie. avi, mov, mpeg, etc) */
    AVOutputFormat* m_format;
    /** A scratch buffer for use the in the video encoding process */
    unsigned char* m_videoOutbuf;
    /** Size of the video output buffer */
    int m_videoOutbufSize;
    /** Context for image format conversion */
    struct SwsContext* m_convertContext;
    AVFrame* m_picture;
};
    
} // namespace vision
} // namespace ram	

#endif // RAM_FFMPEGRECORDER_H_02_24_2008

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 John Edmonds
 * All rights reserved.
 *
 * Author: John "Jack" Edmonds <pocketcookies2@gmail.com>
 * File:  packages/vision/include/FFMPEGCamera.h
 */


// STD Includes
#define __STDC_CONSTANT_MACROS
#include <cstring>
#include <stdint.h>

// Library Includes
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

// Project Includes
#include "vision/include/FFMPEGCamera.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

FFMPEGCamera::FFMPEGCamera(std::string filename) :
    m_width(0),
    m_height(0),
    m_fps(0),
    m_duration(0),
    m_currentTime(0),
    m_timeBase(0),
    m_videoStreamIndex(0),
    m_formatContext(0),
    m_codecContext(0),
    m_frame(0),
    m_RGBframe(0),
    m_pictureBuffer(0),
    m_currentFrame(0),
    m_convertContext(0)
{
    int ret = 0;
    this->m_currentFrame = 0;

    // Register all codecs and formats (can be called many times)
    av_register_all();
    
    // Open up the av container and the streams it contains
    ret = av_open_input_file(&m_formatContext, filename.c_str(), NULL, 0,
                             NULL);
    assert(0 == ret && "Error opening given file");
    
    ret = av_find_stream_info(m_formatContext);
    assert(ret >= 0 && "Error opening stream inside file");
    dump_format(m_formatContext, 0, filename.c_str(), false);

    // Attempt to find a video stream
    m_videoStreamIndex = -1;
    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++)
    {
        if (m_formatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
        {
            m_videoStreamIndex = i;
            break;
        }
    }
    assert(m_videoStreamIndex != -1 && "Could not find video stream in file");

    // Determine the duration
    int64_t rawDuration =
        m_formatContext->streams[m_videoStreamIndex]->duration;
    AVRational rawTimeBase =
        m_formatContext->streams[m_videoStreamIndex]->time_base;
    m_timeBase = av_q2d(rawTimeBase);
    m_duration = rawDuration * m_timeBase;

    // Look up the frame rate
    m_fps = av_q2d(m_formatContext->streams[m_videoStreamIndex]->r_frame_rate);

    // Grab the codec for the stream and make sure we can decode it
    m_codecContext = m_formatContext->streams[m_videoStreamIndex]->codec;
    AVCodec* pCodec = avcodec_find_decoder(m_codecContext->codec_id);
    assert(pCodec != NULL && "Error finding decoder for the needed codec");

    /*if (pCodec->capabilities & CODEC_CAP_TRUNCATED)
     m_codecContext->flags |= CODEC_FLAG_TRUNCATED;*/
    ret = avcodec_open(m_codecContext, pCodec);
    assert(ret == 0 && "Error opening codec");

    // Get the width and hieght of our video stream
    m_width = m_codecContext->width;
    m_height = m_codecContext->height;
    
    // Allocate our temporaty storage buffers and images
    m_frame = avcodec_alloc_frame();
    m_RGBframe = avcodec_alloc_frame();

    size_t numBytes = avpicture_get_size(PIX_FMT_RGB24, m_codecContext->width,
                                         m_codecContext->height);
    m_pictureBuffer = (uint8_t*) av_malloc(numBytes * sizeof(uint8_t));
    
    avpicture_fill((AVPicture*) m_RGBframe, m_pictureBuffer, PIX_FMT_RGB24,
                   m_codecContext->width, m_codecContext->height);


    // Create conversion context that goes from the codec color format to BGR
    m_convertContext = sws_getContext(m_codecContext->width,
                                      m_codecContext->height,
                                      m_codecContext->pix_fmt,
                                      m_codecContext->width,
                                      m_codecContext->height,
                                      PIX_FMT_BGR24,
                                      SWS_BICUBIC, NULL, NULL, NULL);
}

FFMPEGCamera::~FFMPEGCamera()
{
    // Have to stop background capture before we release the capture!
    cleanup();
    
    av_free(m_pictureBuffer);
    av_free(this->m_RGBframe);
    av_free(this->m_frame);
    av_close_input_file(this->m_formatContext);
}

void FFMPEGCamera::update(double timestep)
{
    // Grab the next frame
    readNextFrame();

    // Copy image to public side of the interface
    Image* newImage = new OpenCVImage(m_pictureBuffer, 640, 480, false);

    // Notfiy everyone that the image is uploaded
    capturedImage(newImage);

    // Remove the temp image
    delete newImage;
}

    
void FFMPEGCamera::readNextFrame()
{
    static bool firstTime = true;
    static AVPacket pkt= {0};
    int valid = 0;
    int gotPicture = 0;

    // First time we're called, set packet.data to NULL to indicate it
    // doesn't have to be freed
    if (firstTime)
    {
        firstTime = false;
        pkt.data = NULL;
    }

    // Free last packet if exist
    if (pkt.data != NULL)
        av_free_packet(&pkt);

    // Get the next frame
    while ((0 == valid) && (av_read_frame(m_formatContext, &pkt) >= 0)) 
    {
        // If its not a packet from our stream, keep going
        if( pkt.stream_index != m_videoStreamIndex ) continue;

        // Decode the packet
        avcodec_decode_video(m_codecContext, 
                             m_frame, &gotPicture, 
                             pkt.data, pkt.size);

        // See if we have a new frame
        if (gotPicture) 
        {
            // Grab the curretn timestamp
            int64_t timestamp;
            if (pkt.pts != (int)AV_NOPTS_VALUE)
                timestamp = pkt.pts;
            else
                timestamp = pkt.dts;
            m_currentTime = timestamp * m_timeBase;

            // Convert the frame
	    this->m_currentFrame++;
	    sws_scale(m_convertContext, m_frame->data,
		      m_frame->linesize, 0, height(),
		      m_RGBframe->data,
		      m_RGBframe->linesize);
	    valid = 1;
	}
    }
}


size_t FFMPEGCamera::width()
{
    return m_width;
}
    
size_t FFMPEGCamera::height()
{
    return m_height;
}

double FFMPEGCamera::fps()
{
    return m_fps;
}

double FFMPEGCamera::duration()
{
    return m_duration;
}

void FFMPEGCamera::seekToTime(double seconds)
{
}

double FFMPEGCamera::currentTime()
{
    return m_currentTime;
}
    
void FFMPEGCamera::seekBy(int frames) {
    seekTo(this->m_currentFrame += frames);
}

void FFMPEGCamera::seekTo(int frame) {
    int avseekFlag;
    if (frame < this->m_currentFrame)
        avseekFlag = AVSEEK_FLAG_BACKWARD;
    else
        avseekFlag = 0;
        
        this->m_currentFrame = frame;
        int64_t seekTarget = this->m_currentFrame * 2; // THIS 2 is Magic! haven't figured it out yet
//    int64_t seekTarget = av_rescale_q(this->m_currentFrame, AV_TIME_BASE_Q,
//            this->m_formatContext->streams[this->m_videoStreamIndex]->time_base);
        printf("SeekTo, target: %lld\n", seekTarget);
    av_seek_frame(this->m_formatContext, this->m_videoStreamIndex, seekTarget,
            avseekFlag);
    avcodec_flush_buffers(this->m_codecContext);
}

} // namespace vision
} // namespace ram

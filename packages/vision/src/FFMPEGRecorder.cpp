/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/FFMPEGRecorder.cpp
 */

#define INT64_C

#define __STDC_CONSTANT_MACROS

#include <stdint.h>

// STD Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Library Includes
extern "C" {
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

// Project Includes
#include "vision/include/FFMPEGRecorder.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//#undef exit

#define STREAM_FRAME_RATE 5 /* 5 images/s */
#define STREAM_PIX_FMT PIX_FMT_YUV420P /* default pix_fmt */

static int sws_flags = SWS_BICUBIC;

namespace ram {
namespace vision {

FFMPEGRecorder::FFMPEGRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                               std::string filename, int policyArg) :
    Recorder(camera, policy, policyArg),
    m_formatContext(0),
    m_videoStream(0),
    m_format(0),
    m_videoOutbuf(0),
    m_videoOutbufSize(0),
    m_convertContext(0),
    m_picture(0)
{
    assert((RP_START < policy) && (policy < RP_END) &&
           "Invalid recording policy");
    
    // Determine video FPS (default to 30)
    int fps = camera->fps();
    if (0 == fps)
        fps = 30;

    // Initialize libavcodec, and register all codecs and formats
    av_register_all();

    // Auto detect the output format from the name. default is
    // mpeg.
    m_format = guess_format(NULL, filename.c_str(), NULL);
    if (!m_format)
    {
        printf("Could not deduce output format from file extension: using avi.\n");
        m_format = guess_format("avi", NULL, NULL);
    }
    if (!m_format)
    {
        fprintf(stderr, "Could not find suitable output format\n");
        assert(false && "output format");
    }
    //printf("Format gotten\n");

    // Allocate the output media context
    m_formatContext = av_alloc_format_context();
    if (!m_formatContext)
    {
        fprintf(stderr, "Memory error\n");
        assert(false && "Memory error");
    }
    m_formatContext->oformat = m_format;
    snprintf(m_formatContext->filename, sizeof(m_formatContext->filename),
             "%s", filename.c_str());
    
    // set the output parameters (must be done even if no
    // parameters).
    if (av_set_parameters(m_formatContext, NULL) < 0)
    {
        fprintf(stderr, "Invalid output format parameters\n");
        assert(false && "Invalid output format parameters");
    }
    
    // Set CodecID manually
    m_formatContext->video_codec_id = CODEC_ID_MPEG4; //CODEC_ID_MPEG2VIDEO;
    
    // Add the audio and video streams using the default format codecs
    // and initialize the codecs 
    if (m_format->video_codec != CODEC_ID_NONE)
    {
        m_videoStream = addVideoStream(m_formatContext->video_codec_id);//m_format->video_codec);
    }
    else
    {
        assert(false && "No codec chosen");
    }

    
    dump_format(m_formatContext, 0, filename.c_str(), 1);

    // Now that all the parameters are set, we can open the audio and
    // video codecs and allocate the necessary encode buffers 
    openVideo();

    // Open the output file, if needed
    if (!(m_format->flags & AVFMT_NOFILE))
    {
        if (url_fopen(&m_formatContext->pb, filename.c_str(), URL_WRONLY) < 0)
        {
            fprintf(stderr, "Could not open '%s'\n", filename.c_str());
            assert(false && "Could not open file");
        }
    }

    // write the stream header, if any
    av_write_header(m_formatContext);

    // Create context to handle the image conversion from RGB -> required type
    AVCodecContext* c = m_videoStream->codec;
    m_convertContext = sws_getContext(c->width, c->height,
                                      PIX_FMT_BGR24,
                                      c->width, c->height,
                                      c->pix_fmt,
                                      sws_flags, NULL, NULL, NULL);
    
    // Run update as fast as possible
    background(-1);
}

FFMPEGRecorder::~FFMPEGRecorder()
{
    // Stop the background thread and events
    cleanUp();

    // free video stream
    avcodec_close(m_videoStream->codec);

    av_free(m_picture->data[0]);
    av_free(m_picture);
    av_free(m_videoOutbuf);
    
    // Clean up FFMPEG stuff
    av_write_trailer(m_formatContext);

    // free the streams
    for(int i = 0; i < (int)m_formatContext->nb_streams; i++)
    {
        av_freep(&m_formatContext->streams[i]->codec);
        av_freep(&m_formatContext->streams[i]);
    }

    if (!(m_format->flags & AVFMT_NOFILE))
    {
        // close the output file
        url_fclose(m_formatContext->pb);
    }

    // free format context
    av_free(m_formatContext);
}

void FFMPEGRecorder::recordFrame(Image* image)
{
    AVCodecContext*c = m_videoStream->codec;

//    if (frame_count >= STREAM_NB_FRAMES)
//    {
        /* no more frame to compress. The codec has a latency of a few
           frames if using B frames, so we get the last frames by
           passing the same picture again */
//    }
//    else
//    {
        if (c->pix_fmt != PIX_FMT_BGR24)
        {
            /* as we only generate a BGR picture, we must convert it
               to the codec pixel format if needed */

            AVPicture tmpPicture;
            avpicture_fill(&tmpPicture, image->getData(), PIX_FMT_BGR24,
                           image->getWidth(), image->getHeight());
            sws_scale(m_convertContext, tmpPicture.data,
                      tmpPicture.linesize, 0, c->height, m_picture->data,
                      m_picture->linesize);
        } 
//    }

    int ret = 0;
    if (m_formatContext->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= PKT_FLAG_KEY;
        pkt.stream_index= m_videoStream->index;
        pkt.data= (uint8_t *)m_picture;
        pkt.size= sizeof(AVPicture);

        printf("Staring raw frame write\n");
        ret = av_write_frame(m_formatContext, &pkt);
    } else {
        /* encode the image */
        int out_size = avcodec_encode_video(c, m_videoOutbuf, m_videoOutbufSize,
                                            m_picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);

            if (c->coded_frame->pts != (int)AV_NOPTS_VALUE)
            {
                pkt.pts= av_rescale_q(c->coded_frame->pts,
                                      c->time_base, m_videoStream->time_base);
            }
            if(c->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index= m_videoStream->index;
            pkt.data= (uint8_t*)m_videoOutbuf;
            pkt.size= out_size;

            /* write the compressed frame in the media file */
            ret = av_write_frame(m_formatContext, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        fprintf(stderr, "Error while writing video frame\n");
        exit(1);
    }
}

AVStream* FFMPEGRecorder::addVideoStream(int codec_id_)
{
    CodecID codec_id = (CodecID)codec_id_;
    AVStream* videoStream = av_new_stream(m_formatContext, 0);
    if (!videoStream) {
        fprintf(stderr, "Could not alloc stream\n");
        assert(false && "Coult not allocate stream");
    }

    AVCodecContext* c = videoStream->codec;
    c->codec_id = codec_id;
    c->codec_type = CODEC_TYPE_VIDEO;

    /* put sample parameters */
    c->bit_rate = 800000 * 4;
    /* resolution must be a multiple of two */
    c->width = 640;
    c->height = 480;
    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    c->time_base.den = STREAM_FRAME_RATE;
    c->time_base.num = 1;
    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    c->pix_fmt = STREAM_PIX_FMT;
    if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B frames */
        c->max_b_frames = 2;
    }
    if (c->codec_id == CODEC_ID_MPEG1VIDEO){
        /* Needed to avoid using macroblocks in which some coeffs overflow.
           This does not happen with normal video, it just happens here as
           the motion of the chroma plane does not match the luma plane. */
        c->mb_decision=2;
    }
    // some formats want stream headers to be separate
    if(!strcmp(m_formatContext->oformat->name, "mp4") ||
       !strcmp(m_formatContext->oformat->name, "mov") ||
       !strcmp(m_formatContext->oformat->name, "3gp"))
    {
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    return videoStream;    
}

void FFMPEGRecorder::openVideo()
{
    AVCodecContext *c = m_videoStream->codec;

    // find the video encoder
    AVCodec* codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        assert(false && "Codec not found");
    }

    // open the codec 
    if (avcodec_open(c, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        assert(false && "could not open codec");
    }

    if (!(m_formatContext->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        /* buffers passed into lav* can be allocated any way you prefer,
           as long as they're aligned enough for the architecture, and
           they're freed appropriately (such as using av_free for buffers
           allocated with av_malloc) */
        m_videoOutbufSize = 200000;
        m_videoOutbuf = (uint8_t*)av_malloc(m_videoOutbufSize);
    }

    /* allocate the encoded raw picture */
    m_picture = allocPicture(c->pix_fmt, c->width, c->height);
    if (!m_picture)
    {
        fprintf(stderr, "Could not allocate picture\n");
        assert(false && "Could not allocate picture\n");
    }

    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
/*    tmp_picture = NULL;
    if (c->pix_fmt != PIX_FMT_YUV420P) {
        tmp_picture = alloc_picture(PIX_FMT_YUV420P, c->width, c->height);
        if (!tmp_picture) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            exit(1);
        }
        }*/
}

AVFrame* FFMPEGRecorder::allocPicture(int pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (uint8_t*)av_malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}
    
} // namespace vision
} // namespace ram

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FFMPEGnetworkRecorder.h
 */


// Library Includes
extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
    
// Project Includes
#include "vision/include/FFMPEGNetworkRecorder.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"

static int sws_flags = SWS_BICUBIC;

namespace ram {
namespace vision {

FFMPEGNetworkRecorder::FFMPEGNetworkRecorder(
    Camera* camera,
    Recorder::RecordingPolicy policy,
    boost::uint16_t port, int policyArg,
    int recordWidth, int recordHeight) :
    NetworkRecorder(camera, policy, port, policyArg, recordWidth, recordHeight),
    m_codecContext(0),
    m_convertContext(0),
    m_picture(0)
{
    // Stop background thread
    unbackground(true);

    // must be called before using avcodec lib
    avcodec_init();

    // register all the codecs
    avcodec_register_all();
    
    // find the mpeg4 video encoder
    AVCodec *codec = avcodec_find_encoder(CODEC_ID_MPEG4);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        assert(false && "codec not found");
    }

    m_codecContext= avcodec_alloc_context();

    // put sample parameters
    m_codecContext->bit_rate = 400000;
    // Resolution must be a multiple of two
    m_codecContext->width = getRecordingWidth();
    m_codecContext->height = getRecordingHeight();
    // Frames per second
    m_codecContext->time_base= (AVRational){1,5};
    m_codecContext->gop_size = 10; // emit one intra frame every ten frames
    m_codecContext->max_b_frames=1;
    m_codecContext->pix_fmt = PIX_FMT_YUV420P;

    // Open it
    if (avcodec_open(m_codecContext, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        assert(false && "could not open codec");
    }

    // Create conversion context
    m_convertContext = sws_getContext(m_codecContext->width,
                                      m_codecContext->height,
                                      PIX_FMT_BGR24,
                                      m_codecContext->width,
                                      m_codecContext->height,
                                      m_codecContext->pix_fmt,
                                      sws_flags, NULL, NULL, NULL);
    if (!m_convertContext)
    {
        fprintf(stderr, "Could not create conversion context\n");
        assert(false && "Could not create conversion context\n");
    }
    
    // Allocate picture used for encoding
    m_picture = allocPicture(m_codecContext->pix_fmt, m_codecContext->width,
                             m_codecContext->height);
    if (!m_picture)
    {
        fprintf(stderr, "Could not allocate picture\n");
        assert(false && "Could not allocate picture\n");
    }

    // Run update, will wait for connections, then send images over the network
    background(-1);
}

FFMPEGNetworkRecorder::~FFMPEGNetworkRecorder()
{
    // Stop the background thread, events, and network connections
    cleanUp();

    avcodec_close(m_codecContext);
    av_free(m_codecContext);
    av_free(m_picture->data[0]);
    av_free(m_picture);
}

size_t FFMPEGNetworkRecorder::compress(Image* toCompress,
                                       unsigned char* output,
                                       size_t outputSize)
{
    AVFrame tmpPicture;
    avpicture_fill((AVPicture*)&tmpPicture, toCompress->getData(),
                   PIX_FMT_BGR24, toCompress->getWidth(),
                   toCompress->getHeight());
    sws_scale(m_convertContext, tmpPicture.data,
              tmpPicture.linesize, 0, m_codecContext->height,
              m_picture->data,
              m_picture->linesize);
    int size = avcodec_encode_video(m_codecContext, (uint8_t*)output,
                                outputSize, m_picture);
    return size;
}

AVFrame* FFMPEGNetworkRecorder::allocPicture(int pix_fmt, int width, int height)
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

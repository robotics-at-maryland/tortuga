/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Joseph Lisee
 * File:  packages/vision/include/NetworkCamera.cpp
 */

//#include <iostream>
// Library Includes
extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

// Project includes
#include "vision/include/FFMPEGNetworkCamera.h"
#include "vision/include/OpenCVImage.h"


namespace ram {
namespace vision {

FFMPEGNetworkCamera::FFMPEGNetworkCamera(std::string hostname,
                                         boost::uint16_t port) :
    NetworkCamera(hostname, port),
    m_codecContext(0),
    m_convertContext(0),
    m_picture(0)
{
    /* must be called before using avcodec lib */
    avcodec_init();

    /* register all the codecs */
    avcodec_register_all();
    
    /* find the mpeg1 video decoder */
    AVCodec* codec = avcodec_find_decoder(CODEC_ID_MPEG4);
    if (!codec)
    {
        fprintf(stderr, "codec not found\n");
        assert(false && "codec not found\n");
    }

    m_codecContext= avcodec_alloc_context();
    // Resolution must be a multiple of two
    m_codecContext->width = 640;
    m_codecContext->height = 480;

    
    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        m_codecContext->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open(m_codecContext, codec) < 0)
    {
        fprintf(stderr, "could not open codec\n");
        assert(false && "could not open codec\n");
    }


    
    // Create conversion context
    m_convertContext = sws_getContext(m_codecContext->width,
                                      m_codecContext->height,
                                      PIX_FMT_YUV420P,
                                      m_codecContext->width,
                                      m_codecContext->height,
                                      PIX_FMT_BGR24,
                                      SWS_BICUBIC, NULL, NULL, NULL);
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
}

FFMPEGNetworkCamera::~FFMPEGNetworkCamera()
{
    // Have to stop background capture before we release the capture!
    cleanup();

    avcodec_close(m_codecContext);
    av_free(m_codecContext);
    av_free(m_picture->data[0]);
    av_free(m_picture);
}

void FFMPEGNetworkCamera::decompress(unsigned char* compressedBuffer,
                                     size_t compressedSize,
                                     unsigned char* outputBuffer)
{
    // Decompress image
    int gotPicture = 0;
    /*int len =*/ avcodec_decode_video(m_codecContext, m_picture, &gotPicture,
                                   (uint8_t*)compressedBuffer, compressedSize);
//    std::cout << "CLen: " << compressedSize << " Len: " << len
//              << " ELen: " << width() * height() * 3 << " Got: "
//              << gotPicture << std::endl;
//    assert(got_picture && "Error did not get picture");
    // Convert into the output buffer
    AVFrame tmpPicture;
    avpicture_fill((AVPicture*)&tmpPicture,
                   (uint8_t*)outputBuffer, PIX_FMT_BGR24,
                   width(), height());
    sws_scale(m_convertContext, m_picture->data,
              m_picture->linesize, 0, height(),
              tmpPicture.data,
              tmpPicture.linesize);
}

AVFrame* FFMPEGNetworkCamera::allocPicture(int pix_fmt, int width, int height)
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

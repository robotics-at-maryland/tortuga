/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Joseph Lisee
 * File:  packages/vision/include/NetworkCamera.cpp
 */


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
    // must be called before using avcodec lib
    avcodec_init();

    // register all the codecs
    avcodec_register_all();
    
    // find the mpeg1 video decoder
    AVCodec* codec = avcodec_find_decoder(CODEC_ID_MPEG4);
    if (!codec)
    {
        fprintf(stderr, "codec not found\n");
        assert(false && "codec not found\n");
    }

    m_codecContext= avcodec_alloc_context();
    // Resolution must be a multiple of two
    m_codecContext->width = width();
    m_codecContext->height = height();


    // we do not send complete frames
    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        m_codecContext->flags|= CODEC_FLAG_TRUNCATED; 

    // For some codecs, such as msmpeg4 and mpeg4, width and height
    // MUST be initialized there because this information is not
    // available in the bitstream.

    // open it
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
    int gotPicture = 0;
    uint8_t* inbufPtr = compressedBuffer;
    int size = (int)compressedSize;
    int len = -1;

    // Loop through the compressed image buffer grabbing frames when we get
    // them
    while (size > 0)
    {
        // Decompress the input data with our current codec into m_picture
        len = avcodec_decode_video(m_codecContext, m_picture, &gotPicture,
                                   inbufPtr, size);
        if (len < 0)
            assert(false && "Error while decoding frame");
        
        if (gotPicture)
        {
            // We got a new image, convert the result into RGB format placed
            // in the raw pixel outputBuffer
            
            // Temp container wrapping outputBuffer for use in the conversion
            // function
            AVFrame tmpPicture;
            
            avpicture_fill((AVPicture*)&tmpPicture,
                           (uint8_t*)outputBuffer, PIX_FMT_BGR24,
                           width(), height());

            // Convert from m_picture to tmpPicture, placing the result into
            // outputBuffer
            sws_scale(m_convertContext, m_picture->data,
                      m_picture->linesize, 0, height(),
                      tmpPicture.data,
                      tmpPicture.linesize);
        }
        
        size -= len;
        inbufPtr += len;
    }
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

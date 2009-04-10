/*
 * movie.cpp
 *
 *  Created on: Nov 17, 2008
 *      Author: jack
 */

#define __STDC_CONSTANT_MACROS
#include <cstring>
#include <stdint.h>
#include "vision/include/FFMPEGCamera.h"


Movie::Movie(char *filename) {
    this->currentFrame = 0;
    if (av_open_input_file(&pFormatContext, filename, NULL, 0, NULL))
        handleError();
    if (av_find_stream_info(pFormatContext) < 0)
        handleError();
    dump_format(pFormatContext, 0, filename, false);

    videoStreamIndex = -1;
    for (unsigned int i = 0; i < pFormatContext->nb_streams; i++) {
        if (pFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }
    if (videoStreamIndex == -1)
        handleError();

        AVRational rawTimeBase = pFormatContext->streams[videoStreamIndex]->time_base;
        int64_t duration = pFormatContext->streams[videoStreamIndex]->duration;
        double frameRate = av_q2d(pFormatContext->streams[videoStreamIndex]->r_frame_rate);
        double timeBase = av_q2d(rawTimeBase);
        printf("TimeBase: %f (%d/%d), FPS: %f (R: %f) Duration: %f\n",
               timeBase, rawTimeBase.num, rawTimeBase.den, 1/timeBase, frameRate, duration * timeBase);
        
    pCodecContext = pFormatContext->streams[videoStreamIndex]->codec;
    pCodec = avcodec_find_decoder(pCodecContext->codec_id);
    if (pCodec == NULL)
        handleError();
    /*if (pCodec->capabilities & CODEC_CAP_TRUNCATED)
     pCodecContext->flags |= CODEC_FLAG_TRUNCATED;*/
    if (avcodec_open(pCodecContext, pCodec) < 0)
        handleError();

    pFrame = avcodec_alloc_frame();
    pFrameRGB = avcodec_alloc_frame();

    numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecContext->width,
            pCodecContext->height);
    buffer = (uint8_t*) av_malloc(numBytes * sizeof(uint8_t));

    this->width = pCodecContext->width;
    this->height = pCodecContext->height;
    avpicture_fill((AVPicture*) pFrameRGB, buffer, PIX_FMT_RGB24,
            pCodecContext->width, pCodecContext->height);


        m_convertContext = sws_getContext(pCodecContext->width,
                                          pCodecContext->height,
                                          pCodecContext->pix_fmt,
                                          pCodecContext->width,
                                          pCodecContext->height,
                                          PIX_FMT_BGR24,
                                          SWS_BICUBIC, NULL, NULL, NULL);

}

Movie::~Movie() {
    av_free(buffer);
    av_free(this->pFrameRGB);
    av_free(this->pFrame);
    av_close_input_file(this->pFormatContext);
}

void Movie::handleError() {
    printf("%s", "An error occurred.\n");
}

void Movie::readFrame() {
    int valid=0;
    static bool bFirstTime = true;
    static AVPacket pkt;
    static double accuDur = 0;
    int got_picture;
    int64_t timestamp;

    double timeBase = av_q2d(pFormatContext->streams[videoStreamIndex]->time_base);

    // First time we're called, set packet.data to NULL to indicate it
    // doesn't have to be freed
    if (bFirstTime) {
        bFirstTime = false;
        pkt.data = NULL;
    }

    // free last packet if exist
    if (pkt.data != NULL) {
        av_free_packet (&pkt);
    }

    // get the next frame
    while ((0 == valid) && (av_read_frame(pFormatContext, &pkt) >= 0)) 
    {
        if( pkt.stream_index != videoStreamIndex ) continue;

        avcodec_decode_video(pCodecContext, 
                             pFrame, &got_picture, 
                             pkt.data, pkt.size);

	if (pkt.pts != (int)AV_NOPTS_VALUE)
	    timestamp = pkt.pts;
	else
	    timestamp = pkt.dts;
	//gotFrame = 0 != frameFinished;            
	accuDur += pkt.duration * timeBase;
	printf("TS: %lld (P: %lld D: %lld AD: %f) (Pkt. Dur.: %f)",
	       timestamp, pkt.pts, pkt.dts, accuDur, pkt.duration * timeBase);

        if (got_picture) 
        {
            // we have a new picture, so memorize it
	    double time = timestamp * timeBase; 
	    printf(" (Frame #: %d, Time? %f)\n", this->currentFrame + 1, time);

	    this->currentFrame++;
	    sws_scale(m_convertContext, pFrame->data,
		      pFrame->linesize, 0, getHeight(),
		      pFrameRGB->data,
		      pFrameRGB->linesize);
	    valid = 1;
        } else {
            printf("%s", "\n\n");
	}
    }
    
    // return if we have a new picture or not
    //    return valid;
}


int Movie::getNumBytes() {
    return this->numBytes;
}
AVFrame *Movie::getPFrameRGB() {
    return this->pFrameRGB;
}
int Movie::getWidth() {
    return this->width;
}
int Movie::getHeight() {
    return this->height;
}
void Movie::seekBy(int frames) {
    seekTo(this->currentFrame += frames);
/*    int avseekFlag;
    if (frames < 0)
        avseekFlag = AVSEEK_FLAG_BACKWARD;
    else
        avseekFlag = 0;
        this->currentFrame += frames;
        int64_t seekTarget = this->currentFrame * 2;
//    int64_t seekTarget = av_rescale_q(this->currentFrame, AV_TIME_BASE_Q,
//            this->pFormatContext->streams[this->videoStreamIndex]->time_base);
        printf("SeekBy, target: %ld\n", seekTarget);
    av_seek_frame(this->pFormatContext, this->videoStreamIndex, seekTarget,
            avseekFlag);
                        this->flushPackets();*/
}

void Movie::seekTo(int frame) {
    int avseekFlag;
    if (frame < this->currentFrame)
        avseekFlag = AVSEEK_FLAG_BACKWARD;
    else
        avseekFlag = 0;
        
        this->currentFrame = frame;
        int64_t seekTarget = this->currentFrame * 2; // THIS 2 is Magic! haven't figured it out yet
//    int64_t seekTarget = av_rescale_q(this->currentFrame, AV_TIME_BASE_Q,
//            this->pFormatContext->streams[this->videoStreamIndex]->time_base);
        printf("SeekTo, target: %lld\n", seekTarget);
    av_seek_frame(this->pFormatContext, this->videoStreamIndex, seekTarget,
            avseekFlag);
    this->flushPackets();
}


void Movie::flushPackets() {
    avcodec_flush_buffers(this->pCodecContext);
}

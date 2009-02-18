/*
 * movie.cpp
 *
 *  Created on: Nov 17, 2008
 *      Author: jack
 */

#include "movie.h"

Movie::Movie(char *filename) {
	this->currentFrame = 0;
	if (av_open_input_file(&pFormatContext, filename, NULL, 0, NULL))
		handleError();
	if (av_find_stream_info(pFormatContext) < 0)
		handleError();
	dump_format(pFormatContext, 0, filename, false);

	videoStreamIndex = -1;
	for (int i = 0; i < pFormatContext->nb_streams; i++) {
		if (pFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
			videoStreamIndex = i;
			break;
		}
	}
	if (videoStreamIndex == -1)
		handleError();
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
}

Movie::~Movie() {
	av_free(buffer);
	av_free(this->pFrameRGB);
	av_free(this->pFrame);
	av_close_input_file(this->pFormatContext);
}

void Movie::handleError() {
	printf("An error occurred.\n");
}
void Movie::readFrame() {
	int frameFinished = 0;
	AVPacket packet;
	while (av_read_frame(pFormatContext, &packet) >= 0 && !frameFinished) {
		if (packet.stream_index == videoStreamIndex) {
			avcodec_decode_video(pCodecContext, pFrame, &frameFinished,
					packet.data, packet.size);
			if (frameFinished) {
				img_convert((AVPicture *) pFrameRGB, PIX_FMT_RGB24,
						(AVPicture*) pFrame, pCodecContext->pix_fmt,
						pCodecContext->width, pCodecContext->height);
			}
		}
		av_free_packet(&packet);
	}
	this->currentFrame++;
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
	int avseekFlag;
	if (frames < 0)
		avseekFlag = AVSEEK_FLAG_BACKWARD;
	else
		avseekFlag = 0;
	int64_t seekTarget = av_rescale_q(this->currentFrame += frames, AV_TIME_BASE_Q,
			this->pFormatContext->streams[this->videoStreamIndex]->time_base);
	av_seek_frame(this->pFormatContext, this->videoStreamIndex, seekTarget,
			avseekFlag);
	this->flushPackets();
}
void Movie::flushPackets() {
	avcodec_flush_buffers(this->pCodecContext);
}

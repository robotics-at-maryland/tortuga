extern "C" {
#include <avformat.h>
#include <avcodec.h>
}
/*
 * movie.h
 *
 *  Created on: Nov 17, 2008
 *      Author: jack
 */

#ifndef MOVIE_H_
#define MOVIE_H_

class Movie {
private:
	/**
	 * Used by FFMPEG
	 */
	AVFormatContext *pFormatContext;
	/**
	 * Used by FFMPEG
	 */
	AVCodecContext *pCodecContext;
	/**
	 * Used by FFMPEG
	 */
	AVCodec *pCodec;
	/**
	 * Used by FFMPEG
	 */
	AVFrame *pFrame;
	/**
	 * Used by FFMPEG
	 * Note that this should also store the final data to read.
	 */
	AVFrame *pFrameRGB;
	/**
	 * Used by FFMPEG
	 */
	uint8_t *buffer;
	void handleError();
	int numBytes;
	int width, height;
	int videoStreamIndex;
	int currentFrame;
	/**
	 * Used as a helper function for the seeking functions.
	 */
	void flushPackets();
public:
	Movie(char *filename);
	/**
	 * Reads a single frame from the source.
	 * This should store the result in pFrameRGB.
	 */
	void readFrame();
	virtual ~Movie();

	int getNumBytes();
	AVFrame *getPFrameRGB();
	int getWidth();
	int getHeight();
	/**
	 * Changes the current frame by frames many frames.
	 * @param frames The number of frames to seek by.
	 */
	void seekBy(int frames);
	/**
	 * Jumps to a specific frame.
	 * @param frame The frame to jump to.
	 */
	void seekTo(int frame);
};

#endif /* MOVIE_H_ */

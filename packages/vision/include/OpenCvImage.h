/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/include/CvCamera.h
 */

#ifndef RAM_VISION_OPENCVIMAGE_H_06_06_2007
#define RAM_VISION_OPENCVIMAGE_H_06_06_2007

#include "vision/include/Image.h"

namespace ram {
namespace vision {

class OpenCvImage : public Image
{
public:
	OpenCvImage(IplImage*);
	~OpenCvImage();
	/** The raw image data */
    virtual unsigned char* getData();

    /** Width of image in pixels */
    virtual size_t getWidth();

    /** Height of image in pixels */
    virtual size_t getHeight();

    /** Pixel format of the image */
    virtual Image::PixelFormat getPixelFormat();

    /** Change Image data */
    virtual unsigned char* setData(unsigned char* data);

    /** Set width of image in pixels */
    virtual void setWidth(int pixels);

    /** Set height of image in pixels */
    virtual void setHeight(int pixels);

    /** Set pixel format of the image */
    virtual void setPixelFormat(Image::PixelFormat format);
		
	private:
		IplImage* img;
		
		/** Default constructor Private so it can't be called */
		OpenCvImage();
}

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVIMAGE_H_06_06_2007

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/include/CvCamera.h
 */

#include "vision/include/OpenCvImage.h"
#include "highgui.h"
namespace ram
{
namespace vision
{

OpenCvImage::OpenCvImage(IplImage* image)
{
	img=image;
}

OpenCvImage::OpenCvImage()
{
	assert(false && "Should not be called");
}

OpenCvImage::~OpenCvImage()
{
	cvReleaseImage(&img);
}

size_t OpenCvImage::getWidth()
{
	return img->width;
}

size_t OpenCvImage::getHeight()
{
	return img->height;
}

Image::PixelFormat OpenCvImage::getPixelFormat()
{
	///TODO:Make sure this format is actually the format that the image is in.
	return PF_BGR_8;
}

unsigned char* OpenCvImage::setData(unsigned char* data)
{
	cvSetImageData(img, data, cvGetSize(img).width*3);
}

/** Set width of image in pixels */
void setWidth(int pixels)
{
	assert(false && "Not implemented")
}

/** Set height of image in pixels */
void setHeight(int pixels)
{
	assert(false && "Not implemented")
}

/** Set pixel format of the image */
void setPixelFormat(Image::PixelFormat format)
{
	assert(false && "Not implemented")
}

} // namespace vision
} // namespace ram

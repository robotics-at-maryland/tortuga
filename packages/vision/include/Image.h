/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Image.h
 */

#ifndef RAM_VISION_IMAGE_H_05_29_2007
#define RAM_VISION_IMAGE_H_05_29_2007

// Project Includes
#include "packages/vision/include/common.h"

namespace ram {
namespace vision {

/** Simple Abstract Image Class.
 *
 * This class provides a uniform interface to images comming from a variety of
 * places.
 */
class Image
{
public:
    /** The raw image data */
    virtual unsigned char* getData() = 0;

    /** Width of image in pixels */
    virtual size_t getWidth() = 0;

    /** Height of image in pixels */
    virtual size_t getHeight() = 0;

    /** Pixel format of the image */
    virtual Image::PixelFormat getPixelFormat() = 0;

    /** Change Image data */
    virtual void setData(unsigned char* data) = 0;

    /** Set width of image in pixels */
    virtual void getWidth(size_t pixels) = 0;

    /** Set height of image in pixels */
    virtual void setHeight(size_t pixels) = 0;

    /** Set pixel format of the image */
    virtual void setPixelFormat(Image::PixelFormat format) = 0;

    /** Describes the pixels format of our images */
    enum PixelFormat
    {
        PF_START, /** Sentinal Value */
        PF_RGB_8, /** Red Green Blue, 8 bits per channel */
        PF_END,   /** Sentinal Value */
    }
};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_IMAGE_H_05_29_2007

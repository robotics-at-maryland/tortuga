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

// STD Includes
#include <cstddef>

// Library Includes
#include <boost/utility.hpp>

// Project Includes
#include "vision/include/Common.h"

namespace ram {
namespace vision {

/** Simple Abstract Image Class.
 *
 * This class provides a uniform interface to images comming from a variety of
 * places.
 */
class Image : public boost::noncopyable
{
public:
    virtual ~Image() {};
    
    /** Describes the pixels format of our images */
    enum PixelFormat
    {
        PF_START, /** Sentinal Value */
        PF_RGB_8, /** Red Green Blue, 8 bits per channel */
        PF_BGR_8, /** Blue Green Red, 8 bits*/
        PF_END,   /** Sentinal Value */
    };

    /** Copies data from the given image to the src */
    virtual void copyFrom (const Image* src) = 0;
    
    /** The raw image data */
    virtual unsigned char* getData() const = 0;

    /** Width of image in pixels */
    virtual size_t getWidth() const = 0;

    /** Height of image in pixels */
    virtual size_t getHeight() const = 0;

    /** Pixel format of the image */
    virtual Image::PixelFormat getPixelFormat() = 0;

    /** Determines whether or not to release the image buffer */
    virtual bool getOwnership() const = 0;
    
    /** Change Image data (only free if ownership = true) */
    virtual unsigned char* setData(unsigned char* data,
                                   bool ownership = true) = 0;

    /** Set width and height of image in pixels */
    virtual void setSize(int width, int height) = 0;
  
    /** Set pixel format of the image */
    virtual void setPixelFormat(Image::PixelFormat format) = 0;

    /** All images should be castable to IplImage for OpenCV compatibility */
    virtual operator IplImage*() = 0;
};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_IMAGE_H_05_29_2007

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/BasicImage.h
 */

#ifndef RAM_VISION_BASICIMAGE_H_05_29_2007
#define RAM_VISION_BASICIMAGE_H_05_29_2007

// Vision Includes
#include "vision/include/Image.h"

namespace ram {
namespace vision {

/** A simple implementation of the Image Interface */
class BasicImage : public Image
{
public:
    /** Default constructor */
    BasicImage();

    /** Full Constructor.
      *
      *  The image takes owner ship of the given data buffer.  Do not delete it
      *  yourself.
      */
    BasicImage(unsigned char* data, int width, int height,
               bool ownership = true,
               Image::PixelFormat format = Image::PF_RGB_8);

    virtual ~BasicImage();

    /** The raw image data */
    virtual unsigned char* getData() { return m_data; };

    /** Width of image in pixels */
    virtual size_t getWidth() { return m_width; };

    /** Height of image in pixels */
    virtual size_t getHeight() { return m_height; };

    /** Pixel format of the image */
    virtual Image::PixelFormat getPixelFormat() { return m_format; };

    /** Change Image data.
     *
     *  @note This transfers ownership of the exisint data buffer to the caller.
     *
     *  @return You are responsible for deleting this buffer.
     */
    virtual unsigned char* setData(unsigned char* data,
                                   bool ownership = true);

    /** Set width of image in pixels */
    virtual void setWidth(int pixels) = 0;

    /** Set height of image in pixels */
    virtual void setHeight(int pixels) = 0;

    /** Set pixel format of the image */
    virtual void setPixelFormat(Image::PixelFormat format) = 0;

    operator IplImage* ();
    
private:
    /** Raw Image Buffer */
    unsigned char* m_data;
    
    /** Do we own the buffer */
    bool m_own;
    
    /** Width of image */
    size_t m_width;
    
    /** Height of Image */
    size_t m_height;
    
    /** Pixel Format of Image */
    Image::PixelFormat m_format;
    
    /** Used to store IPL Image for casting */
    IplImage* m_iplimage;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_BASICIMAGE_H_05_29_2007

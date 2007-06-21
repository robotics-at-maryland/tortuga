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

class OpenCVImage : public Image
{
public:
    OpenCVImage(int width, int height);
    OpenCVImage(IplImage*, bool ownership = true);
    ~OpenCVImage();

    OpenCVImage& operator= (const OpenCVImage& src);;
  
    /** The raw image data */
    virtual unsigned char* getData();

    /** Width of image in pixels */
    virtual size_t getWidth() const;

    /** Height of image in pixels */
    virtual size_t getHeight() const;

    /** Pixel format of the image */
    virtual Image::PixelFormat getPixelFormat();

    /** Change Image data */
    virtual unsigned char* setData(unsigned char* data,
                                   bool ownership = true);

    /** Set width and height of image in pixels */
    virtual void setSize(int width, int height);

    /** Set pixel format of the image */
    virtual void setPixelFormat(Image::PixelFormat format);
  
  virtual operator IplImage*();
  
private:
    bool m_own;
    IplImage* m_img;
    
    /** Default constructor Private so it can't be called */
    OpenCVImage();
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVIMAGE_H_06_06_2007

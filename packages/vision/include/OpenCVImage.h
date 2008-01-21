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

// STD Includes
#include <string>

// Project Imports
#include "vision/include/Image.h"

// This must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT OpenCVImage : public Image
{
public:
    /** Allocate an image of the desired width and height */
    OpenCVImage(int width, int height);
    
    /** Create an OpenCVImage from the given IplImage */
    OpenCVImage(IplImage*, bool ownership = true);

    /** Creat an OpenCV from the given file */
    OpenCVImage(std::string fileName);
    
    /** Release the underlying OpenCV image if it has ownership */
    ~OpenCVImage();

    virtual void copyFrom (const Image* src);
    
    /** The raw image data */
    virtual unsigned char* getData() const;

    /** Width of image in pixels */
    virtual size_t getWidth() const;

    /** Height of image in pixels */
    virtual size_t getHeight() const;

    /** Pixel format of the image */
    virtual Image::PixelFormat getPixelFormat() const;

    /** Determines whether or not to release the image buffer */
    virtual bool getOwnership() const;
    
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
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVIMAGE_H_06_06_2007

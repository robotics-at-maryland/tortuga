/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/OpenCVImage.h
 */

#ifndef RAM_VISION_OPENCVIMAGE_H_06_06_2007
#define RAM_VISION_OPENCVIMAGE_H_06_06_2007

// STD Includes
#include <string>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Imports
#include "vision/include/Image.h"
#include "math/include/Matrix3.h"

// This must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/**
 * A wrapper around the IplImage type declared in OpenCV.
 *
 * Supports RGB, BGR, YUV444, and Grayscale image formats.
 * If no pixel format is given, the class makes no assumptions and will
 * disable any features that require knowledge of the pixel format.
 * The default value will create the image with 8 bits per channel, 3 channels.
 */
class RAM_EXPORT OpenCVImage : public Image
{
public:
    /** Default constructor */
    OpenCVImage();

    /** Allocate an image of the desired width and height */
    OpenCVImage(int width, int height, Image::PixelFormat fmt = PF_START);
    
    /** Create an OpenCVImage from the given IplImage */
    OpenCVImage(IplImage*, bool ownership = true,
                Image::PixelFormat fmt = PF_START);

    /** Create an image from the given image buffer */
    OpenCVImage(unsigned char* data, int width, int height,
                bool ownership = true, Image::PixelFormat fmt = PF_START);
    
    /** Creat an OpenCV from the given file */
    OpenCVImage(std::string fileName, Image::PixelFormat fmt = PF_START);
    
    /** Release the underlying OpenCV image if it has ownership */
    virtual ~OpenCVImage();

    virtual void copyFrom (const Image* src);
    
    /** The raw image data */
    virtual unsigned char* getData() const;

    /** Width of image in pixels */
    virtual size_t getWidth() const;

    /** Height of image in pixels */
    virtual size_t getHeight() const;

    /** Depth of the image */
    virtual size_t getDepth() const;

    /** Number of channels in the image */
    virtual size_t getNumChannels() const;

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
    
    virtual IplImage* asIplImage() const;
    virtual cv::Mat asMat() const;

private:
    bool m_own;
    unsigned char *m_data;
    IplImage* m_img;
    Image::PixelFormat m_fmt;
    cv::Mat m_imgMat;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVIMAGE_H_06_06_2007

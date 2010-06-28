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
    ~OpenCVImage();

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
    
    /* Here are the steps to convert a BGR pixel to a CIELCH pixel
       assuming a pointer px = &channel 1

       1. invGammaCorrection(px, px + 1, px + 2);
       2. rgb2xyz(px, px + 1, px + 2);
       3. xyz2luv(px, px + 1, px + 2);
       4. luv2lch(px, px + 1, px + 2);

       The pixel channels are now converted to CIELCh
       If gamma correction is turned off, the first step should be ignored
    */

    static void invGammaCorrection(double *ch1, double *ch2, double *ch3);

    void RGB2LCHuv();

    // convert the values of a single pixel from rgb to xyz
    // requires input in range [0, 1]
    static void rgb2xyz(double *r2x, double *g2y, double *b2z);

    // convert the values of a single pixel from xyz to lab
    // requires input processed from rgb2xyz()
    static void xyz2lab(double *x2l, double *y2a, double *z2b);

    // convert the values of a single pixel from xyz to luv
    // requires input processed from rgb2xyz()
    static void xyz2luv(double *x2l, double *y2u, double *z2v);

    // convert the values of a single pixel from lab to lch
    // output is (0, 255)
    // requires input process from xyz2lab() / xyz2luv() respectively
    static void lab2lch_ab(double *l2l, double *a2c, double *b2h);
    static void luv2lch_uv(double *l2l, double *a2c, double *b2h);
    
private:
    void initTransform();

    /** Gets the parameters needed for cvCreateImage from the lookup table */
    void getFormatParameters(const Image::PixelFormat& fmt,
                             int& depth, int& channels);



    // gamma correction factor
    static double gamma;

    // matrix for transforming rgb to xyz
    static math::Matrix3 rgb2xyzTransform;

    // white points for xyz to lab transform
    static double X_ref;
    static double Y_ref;
    static double Z_ref;

    // ref points for xyz to luv transform
    static double u_prime_ref;
    static double v_prime_ref;
    static double eps; // CIE Standard
    static double kappa; // CIE Standard

    static bool initialized;

    bool m_own;
    unsigned char* m_data;
    IplImage* m_img;
    Image::PixelFormat m_fmt;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVIMAGE_H_06_06_2007

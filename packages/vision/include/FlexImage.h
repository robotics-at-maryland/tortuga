/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FlexImage.h
 */

#ifndef RAM_VISION_FLEXIMAGE_H_07_17_2008
#define RAM_VISION_FLEXIMAGE_H_07_17_2008

// Vision Includes
#include "vision/include/Image.h"
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** Implements the image interface but is resizeable
 *
 *  It always make sure its internal buffer is big enough to hold width * height
 *  pixels and it never shrinks it.
 */
class RAM_EXPORT FlexImage : public Image
{
public:
    /** Default constructor */
    FlexImage();

    /** Full Constructor */
    FlexImage(int width, int height,
              Image::PixelFormat format = Image::PF_RGB_8);

    virtual ~FlexImage();

    virtual void copyFrom(const Image* src);

    virtual unsigned char* getData() const;

    virtual size_t getWidth() const { return m_width; }

    virtual size_t getHeight() const { return m_height; }

    virtual Image::PixelFormat getPixelFormat() { return m_format; }

    /** Always returns true */
    virtual bool getOwnership() const { return true; }

    /** This will overwrite any current image data */
    virtual void setSize(int width, int height);
    
    virtual void setPixelFormat(Image::PixelFormat format){ m_format = format; }

    virtual operator IplImage* () { return m_iplimage; }

    virtual IplImage* asIplImage() { return m_iplimage; }
    
private:
    /** Raw Image Buffer */
    unsigned char* m_data;

    /** Size of our data buffer in bytes*/
    size_t m_dataSize;
    
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

#endif // RAM_VISION_FLEXIMAGE_H_07_17_2008

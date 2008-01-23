/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/wxBitmapImage.h
 */

#ifndef RAM_TOOLS_VISIONVWR_WXBITMAPIMAGE_H_01_20_2008
#define RAM_TOOLS_VISIONVWR_WXBITMAPIMAGE_H_01_20_2008

// Project Includes
#include "vision/include/Image.h"

namespace ram {
namespace tools {
namespace visionvwr {

/** Wraps a wxBitmap in an Image class */
class wxBitmapImage : public vision::Image // boost::noncopyable
{
public:
    wxBitmapImage(int width, int height);
    
    virtual ~wxBitmapImage();

    virtual void copyFrom (const Image* src);
    
    //virtual unsigned char* getData() const;

    virtual size_t getWidth() const;

    virtual size_t getHeight() const;

    virtual vision::Image::PixelFormat getPixelFormat() const;

    const wxBitmap* getBitmap();
    
    // Not Implemented yet for the wxBitmap
    virtual unsigned char* getData() const;
    
    virtual bool getOwnership() const;
    
    virtual unsigned char* setData(unsigned char* data,
                                   bool ownership = true);

    virtual void setSize(int width, int height);
  
    virtual void setPixelFormat(vision::Image::PixelFormat format);

    virtual operator IplImage*();

    virtual IplImage* asIplImage() const;
    
private:
    wxBitmap* m_bitmap;
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONVWR_WXBITMAPIMAGE_H_01_20_2008

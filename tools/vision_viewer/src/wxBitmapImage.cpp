/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/src/wxBitmapImage.cpp
 */

// Library Includes
#include <wx/bitmap.h>
#include <wx/rawbmp.h>

// Project Includes
#include "wxBitmapImage.h"

namespace ram {
namespace tools {
namespace visionvwr {

typedef wxPixelData<wxBitmap, wxNativePixelFormat> PixelData;
    
wxBitmapImage::wxBitmapImage(int width, int height) :
    m_bitmap(new wxBitmap(width, height))
{
}

wxBitmapImage::~wxBitmapImage()
{
    delete m_bitmap;
}
    
void wxBitmapImage::copyFrom (const Image* src)
{
    // Handle self copy
    if (this == src)
        return;

    int width = (int)src->getWidth();
    int height = (int)src->getHeight();
    
    // Ensure bitmap is of the proper size
    if ((m_bitmap->GetWidth() != width) || (m_bitmap->GetHeight() != height))
    {
        delete m_bitmap;
        m_bitmap = new wxBitmap(src->getWidth(), src->getHeight());
    }
    
    PixelData bitmapData(*m_bitmap);
    if ( !bitmapData )
    {
        assert(false && "Could not use wxPixelData to convert wxBitmap");
    }

    // Iterator allows cross platform access to bitmap data
    PixelData::Iterator p(bitmapData);

    unsigned char* imageData = src->getData();
    size_t length = width * height;

    // Copy from source to the wxBitmap
    switch(src->getPixelFormat())
    {
        case vision::Image::PF_RGB_8:
        {
            for (size_t i = 0; i < length; ++i)
            {
                p.Red() = *imageData; 
                p.Green() = *(imageData + 1);
                p.Blue() = *(imageData + 2);

                ++p;
                imageData += 3;
            }            
        }
        break;

        case vision::Image::PF_BGR_8:
        {
            for (size_t i = 0; i < length; ++i)
            {
                p.Blue() = *imageData;
                p.Green() = *(imageData + 1);
                p.Red() = *(imageData + 2);

                ++p;
                imageData += 3;
            }
        }
        break;
            
        default:
        {
            assert("Unsupported pixel format");
        }
        break;
    }

    // Copy Other members
    //m_own = src->getOwnership();
}

unsigned char* wxBitmapImage::getData() const
{
    assert(false && "wxBitmapImage::getData Not implemented");
}
    
size_t wxBitmapImage::getWidth() const
{
    return m_bitmap->GetWidth();
}

size_t wxBitmapImage::getHeight() const
{
    return m_bitmap->GetHeight();
}

vision::Image::PixelFormat wxBitmapImage::getPixelFormat() const
{
    return vision::Image::PF_RGB_8;
}

const wxBitmap* wxBitmapImage::getBitmap()
{
    return m_bitmap;
}
    
bool wxBitmapImage::getOwnership() const
{
    assert(false && "wxBitmapImage::getOwnership Not implemented");
}
    
unsigned char* wxBitmapImage::setData(unsigned char* data, bool ownership)
{
    assert(false && "wxBitmapImage::setData Not implemented");
}

void  wxBitmapImage::setSize(int width, int height)
{
    assert(false && "wxBitmapImage::setSize Not implemented");
}

void  wxBitmapImage::setPixelFormat(Image::PixelFormat format)
{
    assert(false && "wxBitmapImage::setPixelFormat Not implemented");
}

wxBitmapImage::operator IplImage* ()
{
    assert(false && "wxBitmapImage::operator IplImage*Not implemented");
}

} // namespace visionvwr
} // namespace tools
} // namespace ram
    

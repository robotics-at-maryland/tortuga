/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/wxBitmapImage.h
 */

#ifndef RAM_TOOLS_VISIONTOOL_WXBITMAPIMAGE_H_01_20_2008
#define RAM_TOOLS_VISIONTOOL_WXBITMAPIMAGE_H_01_20_2008

// Project Includes
#include "vision/include/Image.h"

namespace ram {
namespace tools {
namespace visiontool {

/** Wraps a wxBitmap in an Image class */
class wxBitmapImage: public vision::Image
{
public:
    wxBitmapImage(int width, int height);

    virtual ~wxBitmapImage();

    const wxBitmap* getBitmap();


private:
    wxBitmap* m_bitmap;
};

} // namespace visiontool
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONTOOL_WXBITMAPIMAGE_H_01_20_2008

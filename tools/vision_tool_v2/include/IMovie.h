/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:   tools/vision_tool/include/IMovie.h
 */

#ifndef RAM_TOOLS_VISIONTOOL_IMOVIE
#define RAM_TOOLS_VISIONTOOL_IMOVIE

// STD Includes

// Library Includes

// Project Includes
#include "vision/include/Image.h"
#include "core/include/Forward.h"
#include "core/include/Event.h"
#include "core/include/ReadWriteMutex.h"
#include "Forward.h"

namespace ram {
namespace tools {
namespace visiontool {

class IMovie
{
public:
    virtual ~IMovie() {}

    virtual void setFrame(vision::Image* frame) = 0;

protected:
    IMovie() {}

private:
    virtual void onPaint(wxPaintEvent &event) = 0;
    virtual void onNewImage(core::EventPtr event) = 0;
    virtual void onSize(wxSizeEvent &event) = 0;
};


} // namespace visiontool
} // namespace tools
} // namespace ram


#endif // RAM_TOOLS_VISIONTOOL_IMOVIE

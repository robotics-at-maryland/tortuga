/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:   tools/vision_tool/include/IPLMovie.h
 */

#ifndef RAM_TOOLS_VISION_TOOL_IPLMOVIE
#define RAM_TOOLS_VISION_TOOL_IPLMOVIE

// STD Includes

// Library Includes
#include <wx/window.h>
#include <wx/panel.h>

// Project Includes
#include "vision/include/Image.h"
#include "core/include/Forward.h"
#include "Forward.h"

namespace ram {
namespace tools {
namespace visionvwr {

class IPLMovie : public wxPanel, vision::Image
{
public:
    IPLMovie(wxWindow *parent, Model* model);
    ~IPLMovie();

    // Image methods TODO: Integrate me a different way
    virtual void copyFrom(const Image* src);
    
    virtual size_t getWidth() const;

    virtual size_t getHeight() const;

    virtual size_t getDepth() const;

    virtual size_t getNumChannels() const;
    
    virtual vision::Image::PixelFormat getPixelFormat() const;
    
    const wxBitmap* getBitmap();

    // Not Implemented yet for the wxBitmap
    virtual unsigned char* getData() const;

    virtual bool getOwnership() const;
    
    virtual unsigned char* setData(unsigned char* data, bool ownership =
                                   true);
    
    virtual void setSize(int width, int height);
    
    virtual void setPixelFormat(vision::Image::PixelFormat format);
    
    virtual operator IplImage*();
    
    virtual IplImage* asIplImage() const;
    
    DECLARE_EVENT_TABLE()
    
private:
    void onPaint(wxPaintEvent &event);
    void onNewImage(core::EventPtr event);
    void onSize(wxSizeEvent &event);
    
    /** The source of the new images */
    Model* m_model;

    Image* m_source;
    Image* m_screen;
};


} // namespace visionvwr
} // namespace tools
} // namespace ram


#endif // RAM_TOOLS_VISION_TOOL_IPLMOVIE

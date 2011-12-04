/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:   tools/vision_tool/include/IPLMovie.h
 */

#ifndef RAM_TOOLS_VISIONTOOL_IPLMOVIE
#define RAM_TOOLS_VISIONTOOL_IPLMOVIE

// STD Includes

// Library Includes
#include <wx/window.h>
#include <wx/panel.h>

// Project Includes
#include "vision/include/Image.h"
#include "core/include/Forward.h"
#include "core/include/Event.h"
#include "core/include/ReadWriteMutex.h"
#include "Forward.h"
#include "IMovie.h"

namespace ram {
namespace tools {
namespace visiontool {

class IPLMovie : public IMovie, public wxPanel
{
public:
    IPLMovie(wxWindow *parent, Model* model, 
             core::Event::EventType eventType,
             wxSize size = wxSize(640, 480));

    virtual ~IPLMovie();

    void setFrame(vision::Image* frame);

    DECLARE_EVENT_TABLE()
    
    private:
    void onPaint(wxPaintEvent &event);
    void onNewImage(core::EventPtr event);
    void onSize(wxSizeEvent &event);
    
    core::ReadWriteMutex m_frameMutex;

    /** The source of the new images */
    Model* m_model;
    vision::Image* m_source;
    vision::Image* m_screen;
};


} // namespace visiontool
} // namespace tools
} // namespace ram


#endif // RAM_TOOLS_VISIONTOOL_IPLMOVIE

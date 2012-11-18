/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:   tools/vision_tool/src/IPLMovie.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <wx/dcbuffer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/wx.h>
#include <cv.h>
#include <highgui.h>

#include <boost/bind.hpp>

// Project Includes
#include "IPLMovie.h"
#include "Model.h"

#include "vision/include/Events.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace tools {
namespace visiontool {

BEGIN_EVENT_TABLE(IPLMovie, wxPanel)
    EVT_PAINT(IPLMovie::onPaint)
    EVT_SIZE(IPLMovie::onSize)
END_EVENT_TABLE()

IPLMovie::IPLMovie(wxWindow *parent, Model* model,
                   core::Event::EventType eventType,
                   wxSize size) :
IMovie(),
wxPanel(parent, wxID_ANY, wxDefaultPosition, size),
    m_model(model),
    m_source(new vision::OpenCVImage(size.x, size.y)),
    m_screen(new vision::OpenCVImage(size.x, size.y))
{
    // subscribe to new images coming from the model
    m_model->subscribe(eventType, boost::bind(&IPLMovie::onNewImage, this, _1));
}

IPLMovie::~IPLMovie()
{
    if(m_source != 0)
        delete m_source;
}

void IPLMovie::onPaint(wxPaintEvent &event)
{
    // draw offscreen and then blit to eliminate flicker
    wxBufferedPaintDC dc(this);

    int clientWidth, clientHeight;
    this->GetClientSize(&clientWidth, &clientHeight);

    // wx sucks with sizes not multiples of 4
    clientWidth -= clientWidth % 4;
    clientHeight -= clientHeight % 4;

    // resize the image
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_frameMutex);
        m_screen->copyFrom(m_source);
    }

    m_screen->setSize(clientWidth, clientHeight);

    // copy to a wx formate
    wxImage img(m_screen->getWidth(),
                m_screen->getHeight(),
                m_screen->getData(), true);

    // wx can only display bitmaps so make it a bitmap
    wxBitmap bmp(img);

    // actually draw it
    dc.DrawBitmap(bmp, 0, 0);
}

void IPLMovie::onSize(wxSizeEvent &event)
{
    Refresh();
}

void IPLMovie::setFrame(vision::Image *frame)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_frameMutex);
        m_source->copyFrom(frame);
    }

    Refresh();
}

void IPLMovie::onNewImage(core::EventPtr event)
{
    vision::ImageEventPtr imageEvent =
        boost::static_pointer_cast<vision::ImageEvent>(event); 

    if(!imageEvent)
    {
        std::cerr << "IPLMovie::onNewImage - Invalid Event Type" << std::endl;
        return;
    }

    setFrame(imageEvent->image);
}

} // namespace visiontool
} // namespace tools
} // namespace ram

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
namespace visionvwr {

BEGIN_EVENT_TABLE(IPLMovie, wxPanel)
    EVT_PAINT(IPLMovie::onPaint)
    EVT_SIZE(IPLMovie::onSize)
END_EVENT_TABLE()

IPLMovie::IPLMovie(wxWindow *parent, Model* model) :
wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(640, 480)),
    m_model(model),
    m_source(new vision::OpenCVImage(640, 480)),
    m_screen(new vision::OpenCVImage(640, 480))
{
    // subscribe to new images coming from the model
    m_model->subscribe(Model::NEW_IMAGE,
                       boost::bind(&IPLMovie::onNewImage, this, _1));
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
    m_screen->copyFrom(m_source);
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

void IPLMovie::onNewImage(core::EventPtr event)
{
    vision::ImageEventPtr imageEvent =
        boost::static_pointer_cast<vision::ImageEvent>(event); 

    m_source->copyFrom(imageEvent->image);
    m_source->setPixelFormat(Image::PF_RGB_8);

    // update the display 
    Refresh();
}

unsigned char* IPLMovie::getData() const
{
    assert(false && "IPLMovie::getData Not implemented");
}

void IPLMovie::copyFrom(const Image* src)
{
    assert(false && "IPLMovie::copyFrom Not implemented");
}

size_t IPLMovie::getWidth() const
{
    return m_screen->getWidth();
}

size_t IPLMovie::getHeight() const
{
    return m_screen->getHeight();
}

size_t IPLMovie::getDepth() const
{
    return 8;
}

size_t IPLMovie::getNumChannels() const
{
    return m_source->getNumChannels();
}

vision::Image::PixelFormat IPLMovie::getPixelFormat() const
{
    return vision::Image::PF_RGB_8;
}

const wxBitmap* IPLMovie::getBitmap()
{
    assert(false && "IPLMovie::getBitmap Not implemented");
}

bool IPLMovie::getOwnership() const
{
    assert(false && "IPLMovie::getOwnership Not implemented");
}

unsigned char* IPLMovie::setData(unsigned char* data, bool ownership)
{
    assert(false && "IPLMovie::setData Not implemented");
}

void IPLMovie::setSize(int width, int height)
{
    assert(false && "IPLMovie::setSize Not implemented");
}

void IPLMovie::setPixelFormat(Image::PixelFormat format)
{
    assert(false && "IPLMovie::setPixelFormat Not implemented");
}

IPLMovie::operator IplImage*()
{
    assert(false && "IPLMovie::operator IplImage*Not implemented");
}

IplImage* IPLMovie::asIplImage() const
{
    assert(false && "IPLMovie::asIplImage Not implemented");
}

} // namespace visionvwr
} // namespace tools
} // namespace ram

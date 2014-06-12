/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/src/CameraView.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <wx/dcclient.h>
#include <wx/rawbmp.h>
#include <wx/timer.h>
#include <cv.h>

// Project Includes
#include "CameraView.h"
#include "wxBitmapImage.h"
#include "vision/include/Camera.h"

namespace ram {
namespace tools {
namespace visionvwr {

static int ID_TIMER = 5;
    
BEGIN_EVENT_TABLE(CameraView, wxPanel)
    EVT_PAINT(CameraView::onPaint)
    EVT_CLOSE(CameraView::onClose)
    EVT_TIMER(ID_TIMER, CameraView::onTimer)
END_EVENT_TABLE()

CameraView::CameraView(wxWindow* parent, vision::Camera* camera) :
    wxPanel(parent),
    m_camera(camera),
    m_bitmapImage(new wxBitmapImage(1,1)),
    m_timer(0)
{
    m_timer = new wxTimer(this, ID_TIMER);
    
    if (m_camera)
    {
        // 10 FPS
        m_timer->Start(100);
    }
}

CameraView::~CameraView()
{
    delete m_camera;
    delete m_bitmapImage;
    delete m_timer;
}

void CameraView::setCamera(vision::Camera* camera)
{    
    delete m_camera;
    m_camera = camera;

    m_timer->Stop();
    m_timer->Start(100);
}
    
void CameraView::onPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    // Red Square for debugging purposes
    wxSize size(GetSize());

    // Empty square with red line outline
    dc.SetBrush(*wxWHITE_BRUSH);
    wxPen red(*wxRED_PEN);
    red.SetWidth(1);
    dc.SetPen(red);
    
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    // Now draw out bitmap
    if (m_bitmapImage)
        dc.DrawBitmap(*m_bitmapImage->getBitmap(), 0, 0);
}


void CameraView::onClose(wxCloseEvent& event)
{
    m_timer->Stop();
    event.Skip();
}
    

void CameraView::onTimer(wxTimerEvent& event)
{
    // Get a new image from file
    m_camera->update(0);
    
    // Copy the latest camera image into bitmap
    m_camera->getImage(m_bitmapImage);

    // Force a repaint
    Refresh();
}
    
} // namespace visionvwr
} // namespace tools
} // namespace ram

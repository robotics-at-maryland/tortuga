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
#include <cv.h>

// Project Includes
#include "CameraView.h"
#include "wxBitmapImage.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace tools {
namespace visionvwr {

//static int ID_TIMER = 12312;
    
BEGIN_EVENT_TABLE(CameraView, wxPanel)
    EVT_PAINT(CameraView::onPaint)
//    EVT_CLOSE(CameraView::onClose)
//    EVT_TIMER(ID_TIMER, CameraView::onTimer)
END_EVENT_TABLE()

CameraView::CameraView(wxWindow* parent) ://, vision::Camera* camera)
wxPanel(parent),
//    m_camera(camera)
    m_bitmapImage(0)
{
    m_bitmapImage = new wxBitmapImage(10, 480);

    vision::OpenCVImage tmp("images.jpg");
    m_bitmapImage->copyFrom(&tmp);
}

CameraView::~CameraView()
{
//    delete m_camera;
    delete m_bitmapImage;
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

/*void CameraView::readImage()
{
    
}*/
    
} // namespace visionvwr
} // namespace tools
} // namespace ram

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  sandbox/vision_viewer/src/CameraView.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <wx/dcclient.h>
#include <wx/rawbmp.h>

#include <cv.h>

// Project Includes
#include "CameraView.h"

BEGIN_EVENT_TABLE(CameraView, wxPanel)
    EVT_PAINT(CameraView::onPaint)
END_EVENT_TABLE()

CameraView::CameraView(wxWindow* parent, wxString imageFilename) :
    wxPanel(parent),
    m_bitmap(0)
{
    // Load Image with OpenCV
    if (!imageFilename.IsEmpty())
    {
        IplImage* image = cvLoadImage(imageFilename.mb_str());
        
        // Convert to wxWidgets
        if (image)
        {
            int width = image->width;
            int height = image->height;
            
            // Allocate Bitmap
            m_bitmap = new wxBitmap(width, height);

            // Use low level wxWidgets functions for conversion from
            // BGR to RGB
            
            typedef wxPixelData<wxBitmap, wxNativePixelFormat> PixelData;


            PixelData bitmapData(*m_bitmap);
            if ( !bitmapData )
            {
                std::cout << "ERROR could not use raw bmp" << std::endl;
            }

            PixelData::Iterator p(bitmapData);

            unsigned char* imageData = (unsigned char*)image->imageData;
            size_t length = width * height;
            for (size_t i = 0; i < length; ++i)
            {
                // OpenCV is BGR
                p.Red() = *(imageData + 2);
                p.Green() = *(imageData + 1);
                p.Blue() = *imageData;

                ++p;
                imageData += 3;
            }
            
            cvReleaseImage(&image);
        }
    }
}

CameraView::~CameraView()
{
    delete m_bitmap;
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
    if (m_bitmap)
        dc.DrawBitmap(*m_bitmap, 0, 0);
}

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  sandbox/vision_viewer/src/CameraView.cpp
 */

// Library Includes
#include <wx/dcclient.h>
//#include <wx/bitmap.h>
#include <wx/image.h>
#include <cv.h>

// Project Includes
#include "CameraView.h"

BEGIN_EVENT_TABLE(CameraView, wxPanel)
    EVT_PAINT(CameraView::onPaint)
END_EVENT_TABLE()

CameraView::CameraView(wxWindow* parent, wxString imageFilename) :
    wxPanel(parent),
    m_image(0)
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
            m_image = new wxImage(width, height);

            // Tmp Header to allow for conversion (BGR -> RGB)
            IplImage* tmpImg = cvCreateImageHeader(cvSize(width, height), 8, 3);
            cvSetData(tmpImg, m_image->GetData(), width * 3);

            // This converts directly into the wxImage's memory buffer, so it
            // preforms the copy and conversion in one step
            cvCvtColor((CvArr*)image, tmpImg, CV_BGR2RGB);

            cvReleaseImageHeader(&tmpImg);
            cvReleaseImage(&image);
        }
    }
}

CameraView::~CameraView()
{
    delete m_image;
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

    // Now draw out bitmap (really slow, but just testing right now)
    if (m_image)
    {
        wxBitmap bitmap(*m_image);
        dc.DrawBitmap(bitmap, 0, 0);
    }
}

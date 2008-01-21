/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  sandbox/vision_viewer/include/CameraView.h"
 */

#include <wx/panel.h>
#include <wx/event.h>
class wxBitmap;

#include <highgui.h>

class CameraView : public wxPanel
{
public:
    CameraView(wxWindow* parent, wxString imageFilename);
    
    ~CameraView();
    
    /** Draws the given image on screen */
    void onPaint(wxPaintEvent& event);

private:
    wxBitmap* m_bitmap;
    
    DECLARE_EVENT_TABLE()
};

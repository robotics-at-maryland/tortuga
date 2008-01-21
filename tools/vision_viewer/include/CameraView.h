/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/CameraView.h
 */

#ifndef RAM_TOOLS_VISIONVWR_CAMERAVIEW_H_01_20_2008
#define RAM_TOOLS_VISIONVWR_CAMERAVIEW_H_01_20_2008

// Library Includes
#include <wx/panel.h>
#include <wx/event.h>
#include <highgui.h>

class wxBitmap;

namespace ram {
namespace tools {
namespace visionvwr {

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

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONVWR_CAMERAVIEW_H_01_20_2008

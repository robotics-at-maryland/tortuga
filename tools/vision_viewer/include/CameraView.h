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
#include <wx/timer.h>

// Project Includes
#include "vision/include/Common.h"

namespace ram {
namespace tools {
namespace visionvwr {

class wxBitmapImage;    
    
class CameraView : public wxPanel
{
public:
    CameraView(wxWindow* parent, vision::Camera* camera = 0);
    
    ~CameraView();

    void setCamera(vision::Camera* camera);
    
private:
    /** Draws the given image on screen */
    void onPaint(wxPaintEvent& event);
    
    /** Called on close allows shutdown of timer */
    void onClose(wxCloseEvent& event);
    
    /** Temporary: Upon timer get new image from camera */
    void onTimer(wxTimerEvent& event);

    /** The source of the new images */
    vision::Camera* m_camera;
    
    /** Used to draw the image to the screen */
    wxBitmapImage* m_bitmapImage;

    /** Temp timer */
    wxTimer* m_timer;
   
    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONVWR_CAMERAVIEW_H_01_20_2008

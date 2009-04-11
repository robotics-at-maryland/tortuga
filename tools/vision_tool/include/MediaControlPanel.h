/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 John Edmonds
 * All rights reserved.
 *
 * Author: John "Jack" Edmonds <pocketcookies2@gmail.com>
 * File:   tools/vision_viewer/include/MediaControlPanel.h
 */

#ifndef RAM_TOOLS_VISION_TOOL_MEDIACONTROLPANEL_H_10_05_2008
#define RAM_TOOLS_VISION_TOOL_MEDIACONTROLPANEL_H_10_05_2008

// Library Includes
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

namespace ram {
namespace tools {
namespace visionvwr {
    
class GLMovie;

class MediaControlPanel: public wxPanel
{

public:
    MediaControlPanel(GLMovie *controlledMovie, wxWindow *parent,
                      wxWindowID id = wxID_ANY, const wxPoint &pos =
                      wxDefaultPosition, const wxSize &size = wxDefaultSize);
    ~MediaControlPanel();

DECLARE_EVENT_TABLE()

private:
    wxButton *play;
    wxButton *stop;
    
    GLMovie *controlledMovie;
    
    enum MEDIA_CONTROL_PANEL_BUTTON_IDS
    {
        MEDIA_CONTROL_PANEL_BUTTON_PLAY, MEDIA_CONTROL_PANEL_BUTTON_STOP
    };
    
    void onPlay(wxCommandEvent& event);
    void onStop(wxCommandEvent& event);


};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_MEDIACONTROLPANEL_H_10_05_2008

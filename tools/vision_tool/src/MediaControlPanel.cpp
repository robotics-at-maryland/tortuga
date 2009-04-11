/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 John Edmonds
 * All rights reserved.
 *
 * Author: John "Jack" Edmonds <pocketcookies2@gmail.com>
 * File:   tools/vision_viewer/include/MediaControlPanel.h
 */

// Project Includes
#include "MediaControlPanel.h"
#include "GLMovie.h"

namespace ram {
namespace tools {
namespace visionvwr {
    
BEGIN_EVENT_TABLE(MediaControlPanel, wxPanel)
    EVT_BUTTON(MediaControlPanel::MEDIA_CONTROL_PANEL_BUTTON_PLAY,
                   MediaControlPanel::onPlay)
    EVT_BUTTON(MediaControlPanel::MEDIA_CONTROL_PANEL_BUTTON_STOP,
                   MediaControlPanel::onStop)
END_EVENT_TABLE()

MediaControlPanel::MediaControlPanel(
    GLMovie *controlledMovie, wxWindow *parent, wxWindowID id,
    const wxPoint &pos, const wxSize &size) :
    wxPanel(parent, id, pos, size)
{
    this->controlledMovie = controlledMovie;

    this->play = new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_PLAY,
                              wxT("Play"));
    this->stop = new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_STOP,
                              wxT("Stop"));

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(this->play, 0, 0, 0);
    sizer->Add(this->stop, 0, 0, 0);
    sizer->SetSizeHints(this);
    SetSizer(sizer);
}
MediaControlPanel::~MediaControlPanel()
{
}

void MediaControlPanel::MediaControlPanel::onPlay(wxCommandEvent& event)
{
    if (this->controlledMovie->m_camera != NULL)
        this->controlledMovie->m_timer->Start(100);
}
void MediaControlPanel::MediaControlPanel::onStop(wxCommandEvent& event)
{
    this->controlledMovie->m_timer->Stop();
}

} // namespace visionvwr
} // namespace tools
} // namespace ram

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 John Edmonds
 * All rights reserved.
 *
 * Author: John "Jack" Edmonds <pocketcookies2@gmail.com>
 * File:   tools/vision_viewer/include/MediaControlPanel.h
 */

// STD Includes
#include <cmath>

// Library Includes
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/timer.h>

// Project Includes
#include "MediaControlPanel.h"
#include "GLMovie.h"

#include "vision/include/Camera.h"

namespace ram {
namespace tools {
namespace visionvwr {
    
BEGIN_EVENT_TABLE(MediaControlPanel, wxPanel)
    EVT_BUTTON(MediaControlPanel::MEDIA_CONTROL_PANEL_BUTTON_PLAY,
               MediaControlPanel::onPlay)
    EVT_BUTTON(MediaControlPanel::MEDIA_CONTROL_PANEL_BUTTON_STOP,
               MediaControlPanel::onStop)
END_EVENT_TABLE()

MediaControlPanel::MediaControlPanel(GLMovie *controlledMovie, wxTimer* timer,
                                     wxWindow *parent, wxWindowID id,
                                     const wxPoint &pos, const wxSize &size) :
    wxPanel(parent, id, pos, size),
    m_play(0),
    m_stop(0),
    m_controlledMovie(controlledMovie),
    m_timer(timer)
{
    m_play = new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_PLAY,
                          wxT("Play"));
    m_stop = new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_STOP,
                          wxT("Stop"));
    m_text = new wxStaticText(this, wxID_ANY, wxT(""));
    m_text->SetWindowStyle(wxALIGN_RIGHT);

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_play, 0, 0, 0);
    sizer->Add(m_stop, 0, 0, 0);
    sizer->Add(m_text, 0, wxALIGN_CENTER | wxALL, 5);
    sizer->SetSizeHints(this);
    SetSizer(sizer);
}
    
MediaControlPanel::~MediaControlPanel()
{
}

void MediaControlPanel::update()
{
    updateTimeDisplay();
}

void MediaControlPanel::onPlay(wxCommandEvent& event)
{
    if (m_controlledMovie->m_camera != NULL)
    {
        double fps = m_controlledMovie->m_camera->fps();
        if (fps == 0.0)
            fps = 30;
        m_timer->Start((int)(1000 / fps));
    }
}
    
void MediaControlPanel::onStop(wxCommandEvent& event)
{
    m_timer->Stop();
}

void MediaControlPanel::updateTimeDisplay()
{
    // Split time up into hours minutes and seconds
    int hours;
    int minutes;
    double seconds;
    breakUpTime(m_controlledMovie->m_camera->currentTime(), hours, minutes,
                seconds);

    int totalHours;
    int totalMinutes;
    double totalSeconds;
    breakUpTime(m_controlledMovie->m_camera->duration(), totalHours,
                totalMinutes, totalSeconds);

    
    // Print based on how much information we have
    wxString label;
    if (totalHours != 0)
    {
        label = wxString::Format(wxT("%02d:%02d:%4.2f / %02d:%02d:%4.2f"),
                                 hours, minutes, seconds, totalHours,
                                 totalMinutes, totalSeconds);
    }
    else if (totalMinutes != 0)
    {
        label = wxString::Format(wxT("%02d:%4.2f / %02d:%4.2f"),
                                 minutes, seconds, totalMinutes, totalSeconds);
    }
    else
    {
        label = wxString::Format(wxT("%4.2f / %4.2f"), seconds, totalSeconds);
    }


    
    m_text->SetLabel(label);
}

void MediaControlPanel::breakUpTime(const double inSeconds, int& outHours,
                                    int& outMinutes, double& outSeconds)
{
    double secondsLeft = inSeconds;

    // Determine hours
    outHours = (int)floor(secondsLeft/3600);
    secondsLeft -= outHours * 3600;

    // Determines minutes
    outMinutes = (int)floor(secondsLeft/60);

    // Determine seconds
    outSeconds = secondsLeft - outMinutes * 60;
}

    
} // namespace visionvwr
} // namespace tools
} // namespace ram

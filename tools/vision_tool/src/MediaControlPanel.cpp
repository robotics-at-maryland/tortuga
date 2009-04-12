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
#include <wx/slider.h>

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

MediaControlPanel::MediaControlPanel(wxTimer* timer,
                                     wxWindow *parent, wxWindowID id,
                                     const wxPoint &pos, const wxSize &size) :
    wxPanel(parent, id, pos, size),
    m_slider(0),
    m_text(0),
    m_camera(0),
    m_timer(timer)
{
    wxButton* play = new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_PLAY,
                                  wxT("Play"));
    wxButton* stop = new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_STOP,
                                  wxT("Stop"));
    m_slider = new wxSlider(this, wxID_ANY, 0, 0, 100);
    m_text = new wxStaticText(this, wxID_ANY, wxT(""));
    m_text->SetWindowStyle(wxALIGN_RIGHT);

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(play, 0, 0, 0);
    sizer->Add(stop, 0, 0, 0);
    sizer->Add(m_slider, 1, wxALIGN_CENTER, 0);
    sizer->Add(m_text, 0, wxALIGN_CENTER | wxALL, 5);
    sizer->SetSizeHints(this);
    SetSizer(sizer);
}
    
MediaControlPanel::~MediaControlPanel()
{
}

void MediaControlPanel::update()
{
    if (0 != m_camera)
    {
        updateTimeDisplay();
        m_slider->SetValue((int)(m_camera->currentTime() * m_camera->fps()));
    }
}

void MediaControlPanel::setCamera(vision::Camera* camera)
{
    m_camera = camera;
    
    double fps = m_camera->fps();
    if (fps == 0.0)
        fps = 30;
    double duration = m_camera->duration();
    if (duration == 0.0)
        duration = 100;

    m_slider->SetRange(0, (int)fps*duration);
}
    
void MediaControlPanel::onPlay(wxCommandEvent& event)
{
    if (0 != m_camera)
    {
        double fps = m_camera->fps();
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
    breakUpTime(m_camera->currentTime(), hours, minutes,
                seconds);

    int totalHours;
    int totalMinutes;
    double totalSeconds;
    breakUpTime(m_camera->duration(), totalHours,
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

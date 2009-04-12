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
#include <wx/event.h>

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

MediaControlPanel::MediaControlPanel(GLMovie* movie, wxTimer* timer,
                                     wxWindow *parent, wxWindowID id,
                                     const wxPoint &pos, const wxSize &size) :
    wxPanel(parent, id, pos, size),
    m_slider(0),
    m_text(0),
    m_camera(0),
    m_movie(movie),
    m_timer(timer),
    m_format(FORMAT_SECONDS),
    m_sliderDown(false)
{
    wxButton* play = new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_PLAY,
                                  wxT("Play"));
    wxButton* stop = new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_STOP,
                                  wxT("Stop"));
    m_slider = new wxSlider(this, wxID_ANY, 0, 0, 100);
    m_text = new wxStaticText(this, wxID_ANY, wxT("0.00 / 0.00"));
    m_text->SetWindowStyle(wxALIGN_RIGHT);

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(play, 0, 0, 0);
    sizer->Add(stop, 0, 0, 0);
    sizer->Add(m_slider, 1, wxALIGN_CENTER, 0);
    sizer->Add(m_text, 0, wxALIGN_CENTER | wxALL, 5);
    sizer->SetSizeHints(this);
    SetSizer(sizer);

    // Connect slider events
    Connect(m_slider->GetId(), wxEVT_SCROLL_THUMBTRACK,
            wxScrollEventHandler(MediaControlPanel::onThumbTrack));
    Connect(m_slider->GetId(), wxEVT_SCROLL_THUMBRELEASE,
            wxScrollEventHandler(MediaControlPanel::onThumbRelease));
    Connect(m_slider->GetId(), wxEVT_SCROLL_CHANGED,
            wxScrollEventHandler(MediaControlPanel::onScrollChanged));
}
    
MediaControlPanel::~MediaControlPanel()
{
}

void MediaControlPanel::update()
{
    if (0 != m_camera)
    {
        updateTimeDisplay();
        if (!m_sliderDown)
        {
            m_slider->SetValue(
                (int)(m_camera->currentTime() * m_camera->fps()));
        }
    }
}

void MediaControlPanel::setCamera(vision::Camera* camera)
{
    m_camera = camera;

    // Update slider
    if (!m_sliderDown)
    {
        double fps = m_camera->fps();
        if (fps == 0.0)
            fps = 30;
        double duration = m_camera->duration();
        if (duration == 0.0)
            duration = 100;
        
        m_slider->SetRange(0, (int)fps*duration);
    }

    // Update text display
    determineTimeFormat();
    updateTimeDisplay();
    GetSizer()->RecalcSizes();
    GetSizer()->Layout();
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

void MediaControlPanel::onThumbTrack(wxScrollEvent& event)
{
    m_sliderDown = true;
    updateBasedOnSliderEvent(event);
}

void MediaControlPanel::onThumbRelease(wxScrollEvent& event)
{
    m_sliderDown = false;
}

void MediaControlPanel::onScrollChanged(wxScrollEvent& event)
{
    updateBasedOnSliderEvent(event);
}

void MediaControlPanel::updateBasedOnSliderEvent(wxScrollEvent& event)
{
    if (m_camera)
    {
        double timeStamp = event.GetPosition() / m_camera->fps();
        m_camera->seekToTime(timeStamp);

        if (!m_timer->IsRunning())
        {
            m_movie->nextFrame();
            m_movie->Refresh();
        }

        updateTimeDisplay();
    }
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
    switch (m_format)
    {
        case FORMAT_HOURS:
            label = wxString::Format(wxT("%02d:%02d:%4.2f / %02d:%02d:%4.2f"),
                                     hours, minutes, seconds, totalHours,
                                     totalMinutes, totalSeconds);
            break;
            
        case FORMAT_MINUTES:
            label = wxString::Format(wxT("%02d:%4.2f / %02d:%4.2f"),
                                     minutes, seconds, totalMinutes,
                                     totalSeconds);
            break;

        case FORMAT_SECONDS:
            label = wxString::Format(wxT("%4.2f / %4.2f"), seconds,
                                     totalSeconds);
            break;
    }

    
    m_text->SetLabel(label);
}

void MediaControlPanel::determineTimeFormat()
{
    int totalHours;
    int totalMinutes;
    double totalSeconds;
    breakUpTime(m_camera->duration(), totalHours,
                totalMinutes, totalSeconds);
    
    if (0 != totalHours)
        m_format = FORMAT_HOURS;
    else if (0 != totalMinutes)
        m_format = FORMAT_MINUTES;
    else
        m_format = FORMAT_SECONDS;
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

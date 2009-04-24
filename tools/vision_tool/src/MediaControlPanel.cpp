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
#include <wx/slider.h>
#include <wx/event.h>

#include <boost/bind.hpp>

// Project Includes
#include "MediaControlPanel.h"
#include "Model.h"

#include "vision/include/Camera.h"

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(MediaControlPanel, wxPanel)
END_EVENT_TABLE()
    
MediaControlPanel::MediaControlPanel(Model* model,
                                     wxWindow *parent, wxWindowID id,
                                     const wxPoint &pos, const wxSize &size) :
    wxPanel(parent, id, pos, size),
    m_slider(0),
    m_text(0),
    m_model(model),
    m_format(FORMAT_SECONDS),
    m_sliderDown(false)
{
    wxButton* play = new wxButton(this, wxID_ANY, wxT("Play"));
    wxButton* stop = new wxButton(this, wxID_ANY, wxT("Stop"));
    m_slider = new wxSlider(this, wxID_ANY, 0, 0, 100);
    m_text = new wxStaticText(this, wxID_ANY, wxT("0.00 / 0.00"));
    m_text->SetWindowStyle(wxALIGN_RIGHT);

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(play, 0, wxALL, 3);
    sizer->Add(stop, 0, wxTOP | wxBOTTOM, 3);
    sizer->Add(m_slider, 1, wxALIGN_CENTER | wxTOP | wxBOTTOM, 3);
    sizer->Add(m_text, 0, wxALIGN_CENTER | wxALL, 3);
    sizer->SetSizeHints(this);
    SetSizer(sizer);

    // Subscribe to model events
    m_model->subscribe(
        Model::IMAGE_SOURCE_CHANGED,
        boost::bind(&MediaControlPanel::onImageSourceChanged, this, _1));
    m_model->subscribe(
        Model::NEW_IMAGE,
        boost::bind(&MediaControlPanel::onNewImage, this, _1));
    
    // Connect button event
    Connect(play->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(MediaControlPanel::onPlay));
    Connect(stop->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(MediaControlPanel::onStop));
    
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

void MediaControlPanel::onNewImage(core::EventPtr event)
{
    updateTimeDisplay();
    if (!m_sliderDown)
    {
        m_slider->SetValue(
            (int)(m_model->currentTime() * m_model->fps()));
    }
}

void MediaControlPanel::onImageSourceChanged(core::EventPtr event)
{
    // Update slider
    if (!m_sliderDown)
    {
        double fps = m_model->fps();
        if (fps == 0.0)
            fps = 30;
        double duration = m_model->duration();
        if (duration == 0.0)
        {
            // Live feed, turn off the slider
            m_slider->Disable();
        }
        else
        {
            m_slider->Enable();
            m_slider->SetRange(0, (int)fps*duration);
        }
    }

    // Update text display
    determineTimeFormat();
    updateTimeDisplay();
    GetSizer()->RecalcSizes();
    GetSizer()->Layout();
}
    
void MediaControlPanel::onPlay(wxCommandEvent& event)
{
    m_model->start();
}
    
void MediaControlPanel::onStop(wxCommandEvent& event)
{
    m_model->stop();
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
    double fps = m_model->fps();
    if (fps == 0.0)
        fps = 30;
    
    double timeStamp = event.GetPosition() / fps;
    m_model->seekToTime(timeStamp);
    updateTimeDisplay();
}
    
void MediaControlPanel::updateTimeDisplay()
{
    wxString label;
    
    if (FORMAT_LIVE == m_format)
    {
        label =  wxString::Format(wxT("Live (%02d fps)"), m_model->fps());
    }
    else
    {
        // Split time up into hours minutes and seconds
        int hours;
        int minutes;
        double seconds;
        breakUpTime(m_model->currentTime(), hours, minutes,
                    seconds);
        
        int totalHours;
        int totalMinutes;
        double totalSeconds;
        breakUpTime(m_model->duration(), totalHours,
                    totalMinutes, totalSeconds);
        
        // Print based on how much information we have
        switch (m_format)
        {
            case FORMAT_HOURS:
                label = wxString::Format(
                    wxT("%02d:%02d:%4.2f / %02d:%02d:%4.2f"),
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

            case FORMAT_LIVE:
                label = wxT("ERROR");
                break;
        }
    }

    // Set the label
    m_text->SetLabel(label);
}

void MediaControlPanel::determineTimeFormat()
{
    int totalHours;
    int totalMinutes;
    double totalSeconds;
    double duration = m_model->duration();
    breakUpTime(duration, totalHours, totalMinutes, totalSeconds);

    if (0 == duration)
        m_format = FORMAT_LIVE;
    else if (0 != totalHours)
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

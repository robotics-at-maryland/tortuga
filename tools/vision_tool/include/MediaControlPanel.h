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

// Project Includes
#include "vision/include/Forward.h"

#include "Forward.h"

// Forward Declarations
class wxSlider;
class wxStaticText;
class wxTimer;
class wxScrollEvent;

namespace ram {
namespace tools {
namespace visionvwr {
    
class MediaControlPanel: public wxPanel
{

public:
    MediaControlPanel(GLMovie* m_movie, wxTimer* timer, wxWindow *parent,
                      wxWindowID id = wxID_ANY,
                      const wxPoint &pos = wxDefaultPosition,
                      const wxSize &size = wxDefaultSize);
    ~MediaControlPanel();

    void update();

    void setCamera(vision::Camera* camera);
    
private:
    /** Determines the current type of formatting we are using */
    enum FORMAT_OPTION {
        FORMAT_HOURS,
        FORMAT_MINUTES,
        FORMAT_SECONDS
    };
    
    /** Shows the current movie progress and lets the user adjust it */
    wxSlider* m_slider;
    
    /** Text box displays the current time */
    wxStaticText* m_text;

    /** The object that is decoding the movie */
    vision::Camera* m_camera;

    /** The current movie we are displaying */
    GLMovie* m_movie;
    
    /** The timer that drives the whole process */
    wxTimer* m_timer;

    /** Format string used */
    FORMAT_OPTION m_format;

    /** Records whether or not the slider is down */
    bool m_sliderDown;
    
    enum MEDIA_CONTROL_PANEL_BUTTON_IDS
    {
        MEDIA_CONTROL_PANEL_BUTTON_PLAY, MEDIA_CONTROL_PANEL_BUTTON_STOP
    };

    /** Handler for play button press */
    void onPlay(wxCommandEvent& event);

    /** Handler for stop button press */
    void onStop(wxCommandEvent& event);

    /** Thrown as the user drags the slider, updates movie position */
    void onThumbTrack(wxScrollEvent& event);

    /** Thrown when the user stops dragged the slider, updates movie pos */
    void onThumbRelease(wxScrollEvent& event);

    /** When the user stops dragging the slider, or click on the slider*/
    void onScrollChanged(wxScrollEvent& event);

    /** Updates the movie position based on a slider event */
    void updateBasedOnSliderEvent(wxScrollEvent& event);
    
    /** Updates the displayed time */
    void updateTimeDisplay();

    /** Determine the type of time format we need */
    void determineTimeFormat();
    
    
    /** Breaks up the time into its hours minutes and seconds */
    static void breakUpTime(const double inSeconds, int& outHours,
                            int& outMinutes, double& outSeconds);
    
    
    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_MEDIACONTROLPANEL_H_10_05_2008

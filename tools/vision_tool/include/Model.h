/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/Model.h
 */

#ifndef RAM_TOOLS_VISIONVWR_MODEL_H_04_19_2009
#define RAM_TOOLS_VISIONVWR_MODEL_H_04_19_2009

// STD Includes
#include <string>

// Library Includes
#include <wx/event.h>

// Core Includes
#include "core/include/EventPublisher.h"
#include "vision/include/Forward.h"

class wxTimerEvent;
class wxTimer;

namespace ram {
namespace tools {
namespace visionvwr {

class Model : public core::EventPublisher, public wxEvtHandler
{
public:
    /**
     * \defgroup Model Events
     */
    /* @{ */
    
    /** Signals that we have opened a new image source, like a video or camera*/
    static const core::Event::EventType IMAGE_SOURCE_CHANGED;

    /** Signals there is a new image ready to display */
    static const core::Event::EventType NEW_IMAGE;

    /* @{ */


    Model();
    ~Model();
    
    /**
     * \defgroup Video playback methods
     */
    /* @{ */

    /** Change the current video source to given video file */
    void openFile(std::string filename);
    
    /** Change the current video source to live feed from the desired camera */
    void openCamera(int num = -1);

    /** Stops the automatic sending of new images */
    void stop();

    /** Starts the automatic sending of images */
    void start();

    /** Whether or not the video source is actively playing*/
    bool running();

    /** The framerate of the current video */
    double fps();
    
    /** Get the length of the video (0 for cameras) */
    double duration();
    
    /** Seek to the desired time in the video (does nothing for cameras) */
    void seekToTime(double seconds);
    
    /** Get the current time in the video (0 for cameras) */
    double currentTime();

    /* @{ */
private:
    /** Called when its time to get a new image from the image source */
    void onTimer(wxTimerEvent &event);

    /** Send a NEW_IMAGE event*/
    void sendNewImage();

    /** Send a IMAGE_SOURCE_CHANGED event*/
    void sendImageSourceChanged();
    
    /** Source of the images when send to clients */
    vision::Camera* m_camera;

    /** Timer used to trigger reading of new images */
    wxTimer* m_timer;

    vision::Image* m_newImage;

    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONVWR_MODEL_H_04_19_2009

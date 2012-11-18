/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/Model.h
 */

#ifndef RAM_TOOLS_VISIONTOOL_MODEL_H
#define RAM_TOOLS_VISIONTOOL_MODEL_H

// STD Includes
#include <string>
#include <vector>

// Library Includes
#include <wx/event.h>

// Core Includes
#include "core/include/EventPublisher.h"
#include "core/include/ConfigNode.h"
#include "core/include/Forward.h"
#include "core/include/TimeVal.h"

#include "vision/include/Common.h"

class wxTimerEvent;
class wxTimer;

namespace ram {
namespace tools {
namespace visiontool {

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
    static const core::Event::EventType NEW_RAW_IMAGE;

    /** Signals that the detector has processed the new image */
    static const core::Event::EventType NEW_PROCESSED_IMAGE;

    /** A new 1 channel image representing the first channel in some color space */
    static const core::Event::EventType NEW_CH1_IMAGE;
    static const core::Event::EventType NEW_CH2_IMAGE;
    static const core::Event::EventType NEW_CH3_IMAGE;

    /** A new image that displays the histogram of some channel */
    static const core::Event::EventType NEW_CH1_HIST_IMAGE;
    static const core::Event::EventType NEW_CH2_HIST_IMAGE;
    static const core::Event::EventType NEW_CH3_HIST_IMAGE;
    static const core::Event::EventType NEW_CH12_HIST_IMAGE;
    static const core::Event::EventType NEW_CH23_HIST_IMAGE;    
    static const core::Event::EventType NEW_CH13_HIST_IMAGE;

    /** Trigered whenever a new detector is selected */
    static const core::Event::EventType DETECTOR_CHANGED;

    /** Sent when the properties of a detector have been updated */
    static const core::Event::EventType DETECTOR_PROPERTIES_CHANGED;

    /* @{ */


    Model();
    ~Model();
    
    /** Set the path to the config file to be used for settings
     *
     *  Changing this path will cause the current detector to be recreated 
     *  using settings this config file.  It will also reload firewire cameras
     *  if one is currently in use.
     */
    void setConfigPath(std::string configPath);

    /**
     * \defgroup Video playback methods
     */
    /* @{ */

    /** Change the current video source to given video file */
    void openFile(std::string filename);
    
    /** Change the current video source to live feed from the desired camera */
    void openCamera(int num = -1);

    /** Change the current video source to a network camera */
    void openNetworkCamera(std::string hostname, unsigned int port);

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

    /**
     * \defgroup Detector Methods
     */
    /* @{ */

    /** Gets all the names of the current detectors */
    std::vector<std::string> getDetectorNames();

    /** Changes the detector we are currently using 
     *
     *  @param detectorName
     *      A valid detector name, must be one of the ones returned by
     *      Model::getDetectorNames().
     */
    void changeToDetector(std::string detectorName);

    /** Stops use of the current detector */
    void disableDetector();

    /** Indicate you have changed detector settings 
     *
     *  This cause a reprosses of the latest image, and a sending of a NEW_IMAGE
     *  event so that image we be displayed.
     */
    void detectorPropertiesChanged();

    /** Gets the set of properties for the current detector */
    core::PropertySetPtr getDetectorPropertySet();

    /** Gets the current image */
    vision::Image* getLatestImage();

    /* @} */
    
private:
    /** Fill an image with the histogram of the input */
    void fillHistogram(vision::Image *input, vision::Image *histogram);

    void fill2DHistogram(vision::Image *input, int channel1,
                         int channel2, vision::Image *histogram);
    /** Called when its time to get a new image from the image source */
    void onTimer(wxTimerEvent &event);

    /** */
    
    /** Send a NEW_IMAGE event, after grabing and processing a new image
     *
     *  If a detector is on, this will send the debug image from the detector
     *  after running the latest image through it.  If there is no detector it
     *  will just send the raw camera images.
     *
     *  @param grabFromSource
     *      If true it grabs an from the image source for process and sending.
     */
    void sendNewImage(bool grabFromSource = true);

    /** Send a IMAGE_SOURCE_CHANGED event*/
    void sendImageSourceChanged();
    
    /** Sends the DETECTOR_CHANGED event */
    void sendDetectorChanged();

    /** Attempts to create a detector by using settings from the given config */
    vision::DetectorPtr createDetectorFromConfig(std::string detectorType,
						 core::ConfigNode cfg,
						 std::string& nodeUsed);

    /** Source of the images when send to clients */
    vision::Camera* m_camera;

    /** Timer used to trigger reading of new images */
    wxTimer* m_timer;

    /** The next valid time to send an update */
    core::TimeVal m_nextUpdate;

    /** Time in seconds between updates */
    double m_updateInterval;

    /** A blank image for when we want to show nothing */
    vision::Image* m_blankImage;

    /** The latest image decoded from the image source */
    vision::Image* m_latestImage;

    /** The image we give to the detector itself */
    vision::Image* m_detectorInput;

    /** The image we give as a debug image to the detector*/
    vision::Image* m_detectorOutput;

    vision::Image* m_processingImage;

    /** The single channel image for channel 1 */
    vision::Image* m_ch1Image;
    vision::Image* m_ch2Image;
    vision::Image* m_ch3Image;

    /** The image to store the histogram of a channel */
    vision::Image* m_ch1HistImage;
    vision::Image* m_ch2HistImage;
    vision::Image* m_ch3HistImage;
    vision::Image* m_ch12HistImage;
    vision::Image* m_ch23HistImage;
    vision::Image* m_ch13HistImage;

    /** The current detector we are using to process images */
    vision::DetectorPtr m_detector;

    /** The type of the current detector */
    std::string m_detectorType;

    /** The path to our current config file */
    std::string m_configPath;

    DECLARE_EVENT_TABLE()
};

} // namespace visiontool
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONTOOL_MODEL_H

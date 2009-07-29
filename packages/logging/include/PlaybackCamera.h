/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/PlaybackCamera.h
 */

#ifndef RAM_LOGGING_PLAYBACKCAMERA_H_07_27_2009
#define RAM_LOGGING_PLAYBACKCAMERA_H_07_27_2009

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/Common.h"

namespace ram {
namespace logging {

/** A camera class which pulls images from the feed in real time */
class PlaybackCamera : public ram::vision::Camera
{
public:
    static const std::string PLAYBACK_STR;
    
    PlaybackCamera(vision::CameraPtr camera, core::EventHubPtr eventHub);

    virtual ~PlaybackCamera();

    /** Stop the playback camera from producing images */
    void stop();

    /** Start the playback camera producing images */
    void start();
    
    /** Runs in the background pulling out images from the camera */
    virtual void update(double timestep);
    
    virtual size_t width();

    virtual size_t height();

    virtual double fps();

    virtual double duration();

    virtual void seekToTime(double seconds);

    virtual double currentTime();
    
    virtual bool backgrounded();

    virtual void background(int);

    virtual void unbackground(bool join = false);

protected:
    virtual double getTimeOfDay();
    
    virtual void nextFrameSleep(double seconds);

private:
    /** EventPlayer::STOP handler, stops the playing */
    void stopHandler(core::EventPtr event);

    /** EventPlayer::START handler, starts the playing */
    void startHandler(core::EventPtr event);
    
    vision::CameraPtr m_camera;

    /** Protects access to the next update time */
    core::ReadWriteMutex m_mutex;
    
    double m_nextUpdate;

    double m_updateInterval;
};

}  // namespace logging
}  // namespace ram

#endif // RAM_LOGGING_PLAYBACKCAMERA_H_07_27_2009

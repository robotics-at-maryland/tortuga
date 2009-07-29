/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/PlaybackCamera.h
 */

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "logging/include/PlaybackCamera.h"
#include "logging/include/EventPlayer.h"

#include "core/include/TimeVal.h"
#include "core/include/EventHub.h"

#include "vision/include/CameraMaker.h"

namespace ram {
namespace vision {

// Register the camera with the camera factory    
template <>
vision::CameraPtr vision::CameraMakerTemplate<logging::PlaybackCamera>::
makeObject(
    vision::CameraMakerParamType params)
{
    // The string of the camera we are wrapping for playback
    std::string camInput = params.get<0>().substr(
        logging::PlaybackCamera::PLAYBACK_STR.size(),
        std::string::npos);
    
    // Create the camera we are going to wrap
    vision::CameraPtr toWrap = vision::CameraMaker::newObject(
        vision::CameraMakerParamType(camInput, params.get<1>(), params.get<2>(),
                                     params.get<3>()));

    // Finally create our camera
    return vision::CameraPtr(
        new logging::PlaybackCamera(toWrap, params.get<3>()));
}

static CameraMakerTemplate<logging::PlaybackCamera>
registerPlaybackCamera("PlaybackCamera");

    
} // namespace vision    
} // namespace ram
    
namespace ram {
namespace logging {

const std::string PlaybackCamera::PLAYBACK_STR = "PLAYBACK:";
    
PlaybackCamera::PlaybackCamera(vision::CameraPtr camera,
                               core::EventHubPtr eventHub) :
    m_camera(camera),
    m_nextUpdate(0),
    m_updateInterval(0)
{
    eventHub->subscribeToType(EventPlayer::START,
                              boost::bind(&PlaybackCamera::startHandler, this,
                                          _1));
    eventHub->subscribeToType(EventPlayer::STOP,
                              boost::bind(&PlaybackCamera::stopHandler, this,
                                          _1));

    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
        
        // Determine how much time is between frames
        double fpsNum = fps();
        if (fpsNum == 0.0)
            fpsNum = 30;
        m_updateInterval = 1.0 / fpsNum;
    
        // Determine when we should send off the next frame
        m_nextUpdate = getTimeOfDay() + m_updateInterval;
    }
}

PlaybackCamera::~PlaybackCamera()
{
}

void PlaybackCamera::stop()
{
    // Stop the background thread
    unbackground(true);
}

void PlaybackCamera::start()
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    
    // Determine how much time is between frames
    double fpsNum = fps();
    if (fpsNum == 0.0)
        fpsNum = 30;
    m_updateInterval = 1.0 / fpsNum;

    // Determine when we should send off the next frame
    m_nextUpdate = getTimeOfDay() + m_updateInterval;

    // Start the background thread
    background(-1);
}
   
void PlaybackCamera::update(double timestep)
{
    double sleepTime;

    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
        
        // Send an image
        m_camera->update(0);
    
        // Advance the update the standard ammount
        m_nextUpdate += m_updateInterval;
    
        // Advance out next update until its in the future
        while ((m_nextUpdate - getTimeOfDay()) < 0)
        {
            m_nextUpdate += m_updateInterval;
            m_camera->seekToTime(currentTime() + m_updateInterval);
        }
    
        // Sleep until that time
        sleepTime = m_nextUpdate - getTimeOfDay();
    }
    
    nextFrameSleep(sleepTime);
}
    
size_t PlaybackCamera::width()
{
    return m_camera->width();
}

size_t PlaybackCamera::height()
{
    return m_camera->height();
}

double PlaybackCamera::fps()
{
    return m_camera->fps();
}

double PlaybackCamera::duration()
{
    return m_camera->duration();
}

void PlaybackCamera::seekToTime(double seconds)
{
    m_camera->seekToTime(seconds);
}

double PlaybackCamera::currentTime()
{
    return m_camera->currentTime();
}
    
bool PlaybackCamera::backgrounded()
{
    return core::Updatable::backgrounded();
}

void PlaybackCamera::background(int interval)
{
    core::Updatable::background(interval);
}

void PlaybackCamera::unbackground(bool join)
{
    core::Updatable::unbackground(join);
}

double PlaybackCamera::getTimeOfDay()
{
    return core::TimeVal::timeOfDay().get_double();
}
    
void PlaybackCamera::nextFrameSleep(double seconds)
{
    waitForUpdate((long)(seconds * 1000000));
}

void PlaybackCamera::stopHandler(core::EventPtr event)
{
    stop();
}

void PlaybackCamera::startHandler(core::EventPtr event)
{
    start();
}
    
}  // namespace logging
}  // namespace ram

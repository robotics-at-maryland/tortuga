/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Recorder.cpp
 */


// STD Includes
#include <algorithm>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Recorder.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

#include "core/include/TimeVal.h"
#include "core/include/EventConnection.h"

namespace ram {
namespace vision {

Recorder::Recorder(Camera* camera, Recorder::RecordingPolicy policy,
                   int policyArg) :
    m_policy(policy),
    m_policyArg(policyArg),
    m_newFrame(false),
    m_nextFrame(new OpenCVImage(640, 480)),
    m_currentFrame(new OpenCVImage(640, 480)),
    m_camera(camera),
    m_currentTime(0),
    m_nextRecordTime(0)
{
    assert((RP_START < policy) && (policy < RP_END) &&
           "Invalid recording policy");
    
    // Subscribe to camera event
    m_connection = camera->subscribe(Camera::IMAGE_CAPTURED,
                                     boost::bind(&Recorder::newImageCapture,
                                                 this,
                                                 _1));
}

Recorder::~Recorder()
{
    // Stop the background thread and events
    assert(!backgrounded() && "Recorder::cleanUp() not called by subclass");
    assert(!m_connection->connected() &&
           "Recorder::cleanUp() not called by subclass");
    
    delete m_nextFrame;
    delete m_currentFrame;
}

void Recorder::update(double timeSinceLastUpdate)
{
    m_currentTime += timeSinceLastUpdate;
    
    switch(m_policy)
    {
        case RP_START:
            assert(false && "Invalid recording policy");
            break;

        case MAX_RATE:
        {
            // Don't record if not enough time has passed
            if (m_currentTime < m_nextRecordTime)
            {
                break;
            }
            else
            {
                // Calculate the next time to record
                m_nextRecordTime += (1 / (double)m_policyArg);
            }
        }
        
        // FALL THROUGH - based on current time
            
        case NEXT_FRAME:
        {
            bool frameToRecord = false;
            
            {
                boost::mutex::scoped_lock lock(m_mutex);
                // Check to see if we have a new frame waiting
                if (m_newFrame)
                {
                    // If we have a new frame waiting, swap it for the current
                    frameToRecord = true;
                    std::swap(m_nextFrame, m_currentFrame);
                    m_newFrame = false;
                }
            }

            if (frameToRecord)
            {
                recordFrame(m_currentFrame);
            }
            else
            {
                // If camera is not backgrounded, sleep for 1/30 a second
                if (m_camera->backgrounded())
                    waitForImage(m_camera);
                // Only sleep if we are operting backgrounded
                else if (backgrounded())
                    core::TimeVal::sleep(1.0/30.0);
            }
        }
        break;

        case RP_END:
            assert(false && "Invalid recording policy");
            break;
    }
}

void Recorder::background(int interval)
{
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_newFrame = false;
    }
    Updatable::background(interval);
}
    
void Recorder::cleanUp()
{
    m_connection->disconnect();
    unbackground(true);    
}

void Recorder::waitForImage(Camera* camera)
{
    camera->waitForImage(0);
}
    
void Recorder::newImageCapture(core::EventPtr event)
{
    boost::mutex::scoped_lock lock(m_mutex);

    // Copy new image to our local buffer
    m_newFrame = true;
    m_nextFrame->copyFrom(
        boost::dynamic_pointer_cast<ImageEvent>(event)->image);
}

} // namespace vision
} // namespace ram

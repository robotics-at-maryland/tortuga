/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/Recorder.cpp
 */


// STD Includes
#include <algorithm>

// Library Includes
#include "cv.h"
#include "highgui.h"
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
                   std::string filename) :
    m_policy(policy),
    m_newFrame(false),
    m_nextFrame(new OpenCVImage(640, 480)),
    m_currentFrame(new OpenCVImage(640, 480)),
    m_camera(camera),
    m_writer(0)
{
    assert((RP_START < policy) && (policy < RP_END) &&
           "Invalid recording policy");
    
    // Determine video FPS (default to 30)
    int fps = m_camera->fps();
    if (0 == fps)
        fps = 30;

    CvSize size = cvSize(m_camera->width(), m_camera->height());
    
    FILE* video = fopen(filename.c_str(), "w");
    // Yes, thats a comma, not a semicolon, it forces the compiler to close
    // the file before attempting the next step, creating a video writer on a
    // file that may not exist before the close finishes
    fclose(video),
	m_writer=cvCreateVideoWriter(filename.c_str(),
                                     CV_FOURCC('D','I','V','X'),
                                     fps, size, 1);

    // Subscribe to camera event
    m_connection = camera->subscribe(Camera::IMAGE_CAPTURED,
                                     boost::bind(&Recorder::newImageCapture,
                                                 this,
                                                 _1));
    
    // Run update as fast as possible
    background(-1);
}

Recorder::~Recorder()
{
    // Stop the background thread and events
    m_connection->disconnect();
    unbackground(true);
    
    cvReleaseVideoWriter(&m_writer);
    delete m_nextFrame;
    delete m_currentFrame;
}

void Recorder::update(double timeSinceLastUpdate)
{
    switch(m_policy)
    {
        case RP_START:
            assert(false && "Invalid recording policy");
            break;
        
        case NEXT_FRAME:
        {
            bool recordFrame = false;
            
            {
                boost::mutex::scoped_lock lock(m_mutex);
                // Check to see if we have a new frame waiting
                if (m_newFrame)
                {
                    // If we have a new frame waiting, swap it for the current
                    recordFrame = true;
                    std::swap(m_nextFrame, m_currentFrame);
                    m_newFrame = false;
                }
            }

            if (recordFrame)
            {
                cvWriteFrame(m_writer, m_currentFrame->asIplImage());
            }
            else
            {
                // If camera is not backgrounded, sleep for 1/30 a second
                if (m_camera->backgrounded())
                    m_camera->waitForImage(0);
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

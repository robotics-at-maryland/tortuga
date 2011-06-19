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
#include <sstream>
#include <iostream>

// Library Includes
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Recorder.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

#include "vision/include/FileRecorder.h"
#include "vision/include/RawFileRecorder.h"
#include "vision/include/NetworkRecorder.h"

#include "core/include/TimeVal.h"
#include "core/include/EventConnection.h"

namespace ba = boost::algorithm;
namespace bfs = boost::filesystem;

namespace ram {
namespace vision {

Recorder::Recorder(Camera* camera, Recorder::RecordingPolicy policy,
                   int policyArg, int recordWidth, int recordHeight) :
    m_policy(policy),
    m_policyArg(policyArg),
    m_width(recordWidth),
    m_height(recordHeight),
    m_newFrame(false),
    m_camera(camera),
    m_frameFromCamera(new OpenCVImage(camera->width(), camera->height())),
    m_frameResized(new OpenCVImage(recordWidth, recordHeight)),
    m_currentTime(0),
    m_nextRecordTime(0)
{
    assert((RP_START < policy) && (policy < RP_END) &&
           "Invalid recording policy");
    
    // Subscribe to camera event
    m_connection = camera->subscribe(
        Camera::IMAGE_CAPTURED,
        boost::bind(&Recorder::newImageCapture, this, _1));
}

Recorder::~Recorder()
{
    // Stop the background thread and events
    assert(!backgrounded() && !m_connection->connected() &&
           "Recorder::cleanUp() not called by subclass");
    
    delete m_frameResized;
    delete m_frameFromCamera;
}

void Recorder::update(double timeSinceLastUpdate)
{
    m_currentTime += timeSinceLastUpdate;
    
    switch(m_policy)
    {
        case MAX_RATE:
            // Don't record if not enough time has passed
            if (m_currentTime < m_nextRecordTime)
            {
                break;
            }
            else
            {
                // Calculate the next time to record
                m_nextRecordTime += (1 / static_cast<double>(m_policyArg));
            }
        
        // FALL THROUGH - based on current time
            
        case NEXT_FRAME:
            // Check to see if we have a new frame waiting
            if (m_newFrame)
            {
                // Get a working copy of new frame from the camera
                m_camera->getImage(m_frameFromCamera);
                m_frameFromCamera->setSize(m_width, m_height);
                recordFrame(m_frameFromCamera);
                {
                    boost::mutex::scoped_lock lock(m_mutex);
                    m_newFrame = false;
                }
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
            break;

        default:
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

size_t Recorder::getRecordingWidth() const
{
    return m_width;
}

size_t Recorder::getRecordingHeight() const
{
    return m_height;
}

Recorder* Recorder::createRecorderFromString(const std::string& str,
                                             Camera* camera,
                                             std::string& message,
                                             Recorder::RecordingPolicy policy,
                                             int policyArg,
                                             std::string recorderDir)
{
    static boost::regex port("(\\d{1,5})");
    static boost::regex typeArg("([^(]+)(\\(([^)]+)\\))?");
    std::stringstream ss;
    
    // Parse out the type str and the arguments
    boost::smatch matcher;
    if (!boost::regex_match(str, matcher, typeArg))
    {
        std::cerr << "Invalid record string: " << str << std::endl;
        return 0;
    }
    std::string typeStr = matcher[1];

    // Now split up the arguments
    std::vector<std::string> args;
    std::string argStr = matcher[3];
    if (argStr.size() > 0)
        ba::split(args, argStr, ba::is_any_of(","));
    

    // The first two args are always size
    if (args.size() == 1u)
    {
        std::cerr << "Invalid number of args: " << args.size() << std::endl;
        return 0;
    }
    int width = 640;
    int height = 480;
    if (args.size() >= 2u)
    {
        width = boost::lexical_cast<int>(args[0]);
        height = boost::lexical_cast<int>(args[1]);
        assert(width > 0 && "Record width must be positive");
        assert(height > 0 && "Record height must be positive");
    }

    ss << "Size: (" << width << ", " << height << ") ";
    
    // Now lets create the recorder
    Recorder* recorder = 0;
    
    boost::smatch portMatcher;
    if (boost::regex_match(typeStr, portMatcher, port))
    {
        ss << "Recording to host : '" << boost::lexical_cast<int>(typeStr)
           << "'";

        boost::uint16_t portNum = boost::lexical_cast<boost::uint16_t>(typeStr);

        recorder =
            new vision::NetworkRecorder(camera, policy, portNum,
                                        policyArg, width, height);
    }

    if (!recorder)
    {
        std::string fullPath = (bfs::path(recorderDir) / typeStr).string();
        std::string extension = bfs::path(typeStr).extension();

        ss <<"Assuming string is a file, Recording to '" << typeStr << "'";
        
        if (".rmv" == extension)
        {
            ss << " as raw .rmv";
            recorder = new vision::RawFileRecorder(camera, policy, fullPath,
                                                   policyArg, width, height);
        }
        else
        {
            ss << " as a MPEG4 compressed .avi";
            recorder = new vision::FileRecorder(camera, policy, fullPath,
                                                policyArg, width, height);
        }
    }

    message = ss.str();
    return recorder;
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
    // Do as little as possible here.  Let the recorder know a
    // new frame is available.
    boost::mutex::scoped_lock lock(m_mutex);
    m_newFrame = true;
}

} // namespace vision
} // namespace ram

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/FileRecorder.cpp
 */

// STD Includes
#include <stdio.h>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/FileRecorder.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"

namespace ram {
namespace vision {

FileRecorder::FileRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                           std::string filename, int policyArg,
                           int recordWidth, int recordHeight) :
    Recorder(camera, policy, policyArg, recordWidth, recordHeight),
    m_writer(filename, CV_FOURCC('D','I','V','X'),
             0 == static_cast<int>(camera->fps()) ? 30 : static_cast<int>(camera->fps()),
             cv::Size(getRecordingWidth(), getRecordingHeight()))
{
    assert((RP_START < policy) && (policy < RP_END) &&
           "Invalid recording policy");
    
    // Run update as fast as possible
    background(-1);
}

FileRecorder::~FileRecorder()
{
    // Stop the background thread and events
    cleanUp();
}

void FileRecorder::recordFrame(Image* image)
{
    m_writer << image->asIplImage();
}

} // namespace vision
} // namespace ram

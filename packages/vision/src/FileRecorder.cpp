/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/FileRecorder.cpp
 */


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
    m_writer(0)
{
    assert((RP_START < policy) && (policy < RP_END) &&
           "Invalid recording policy");
    
    // Determine video FPS (default to 30)
    int fps = (int)(camera->fps());
    if (0 == fps)
        fps = 30;

    CvSize size = cvSize(getRecordingWidth(), getRecordingHeight());
    
    FILE* video = fopen(filename.c_str(), "w");
    // Yes, thats a comma, not a semicolon, it forces the compiler to close
    // the file before attempting the next step, creating a video writer on a
    // file that may not exist before the close finishes
    fclose(video),
	m_writer=cvCreateVideoWriter(filename.c_str(),
                                     CV_FOURCC('D','I','V','X'),
                                     fps, size, 1);

    // Run update as fast as possible
    background(-1);
}

FileRecorder::~FileRecorder()
{
    // Stop the background thread and events
    cleanUp();

    // Clean up OpenCV stuff
    cvReleaseVideoWriter(&m_writer);
}

void FileRecorder::recordFrame(Image* image)
{
    cvWriteFrame(m_writer, image->asIplImage());    
}

} // namespace vision
} // namespace ram

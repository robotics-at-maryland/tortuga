/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/MockRecorder.h
 */

#ifndef RAM_MOCKRECORDER_H_02_24_2008
#define RAM_MOCKRECORDER_H_02_24_2008

// Project Includes
#include "vision/include/Recorder.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"

// Must be included last
#include "vision/include/Export.h"

class RAM_EXPORT MockRecorder : public ram::vision::Recorder
{
  public:
    MockRecorder(ram::vision::Camera* camera,
                 ram::vision::Recorder::RecordingPolicy policy) :
        ram::vision::Recorder(camera, policy),
        lastRecordedFrame(0)
    {};

    virtual ~MockRecorder()
    {
        cleanUp();
    }

    ram::vision::Image* lastRecordedFrame;
  protected:

    virtual void recordFrame(ram::vision::Image* image)
    {
        lastRecordedFrame = image;
    }
};
    
#endif // RAM_MOCKRECORDER_H_02_24_2008

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

// STD Includes
#include <set>
#include <algorithm>

// Library Includes
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>

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
                 ram::vision::Recorder::RecordingPolicy policy,
                 int policyArg = 0) :
        ram::vision::Recorder(camera, policy, policyArg),
        lastRecordedFrame(0)
    {};

    virtual ~MockRecorder()
    {
        cleanUp();
    }

    std::set<boost::uint16_t> imageCRCs;
    ram::vision::Image* lastRecordedFrame;
  protected:

    virtual void recordFrame(ram::vision::Image* image)
    {
        // Record the image
        lastRecordedFrame = image;

        // Record the CRC
        unsigned char* data = image->getData();
        std::size_t data_len = image->getWidth() * image->getHeight() * 3;

        boost::crc_16_type crc_it;
        crc_it = std::for_each( data, data + data_len, crc_it);

        imageCRCs.insert(crc_it());
    }
};
    
#endif // RAM_MOCKRECORDER_H_02_24_2008

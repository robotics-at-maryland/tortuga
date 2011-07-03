/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/RawFileRecorder.h
 */


// STD Includes
#include <cstdio>

// Project Includes
#include "vision/include/RawFileRecorder.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"

namespace ram {
namespace vision {

const boost::uint32_t RawFileRecorder::MAGIC_NUMBER = 0xC1A55AC5;
const boost::uint32_t RawFileRecorder::RMV_VERSION = 2;
    
RawFileRecorder::RawFileRecorder(Camera* camera,
                                 Recorder::RecordingPolicy policy,
                                 std::string filename, int policyArg,
                                 int recordWidth, int recordHeight) :
    Recorder(camera, policy, policyArg, recordWidth, recordHeight),
    m_file(0),
    m_framenum(0),
    m_lastPacketSizeWritten(0)
{
    assert((RP_START < policy) && (policy < RP_END) &&
           "Invalid recording policy");

    // Open our video file
    m_file = fopen(filename.c_str(), "w");
    assert(m_file && "Error opening file");
    
    // Determine video FPS (default to 30)
    double fps = 30;
    if (policy == NEXT_FRAME)
    {
        fps = camera->fps();
        if (0 == fps)
            fps = 30;
    }
    else if (policy == MAX_RATE)
    {
        fps = policyArg;
    }

    // Fill in the video header
    Header header = {0};
    header.magicNumber = MAGIC_NUMBER;
    header.width = getRecordingWidth();
    header.height = getRecordingHeight();
    /// TODO: Fix assumption about file bytes
    header.packetSize = header.width * header.height * 3 + sizeof(Packet);
    /// TODO: Fix assumption about BGR format
    header.format = (boost::uint32_t)Image::PF_BGR_8;
    header.framerate = fps;
    header.versionNumber = RMV_VERSION;
    
    // Write the header out to disk then flush to disk
    size_t written = fwrite(&header, sizeof(Header), 1, m_file);
    assert(written == 1 && "Error writing to disk");
    fflush(m_file);

    // Run update as fast as possible
    background(-1);
}

RawFileRecorder::~RawFileRecorder()
{
    // Stop the background thread and events
    cleanUp();

    // Close our file
    fclose(m_file);
}

void RawFileRecorder::recordFrame(Image* image)
{
    image->setPixelFormat(Image::PF_BGR_8);

    // Pack up the header
    Packet packet;
    packet.magicNumber = MAGIC_NUMBER;
    packet.lastPacketSize = m_lastPacketSizeWritten;
    packet.dataSize = image->getWidth() * image->getHeight() * 3;
    packet.framenum = m_framenum;
    
    // Write the header
    size_t written = fwrite(&packet, sizeof(Packet), 1, m_file);
    assert(written == 1 && "Error packet header to disk");
    m_lastPacketSizeWritten = written * sizeof(Packet);
    
    // Write the date
    written = fwrite(image->getData(), packet.dataSize, 1, m_file);
    assert(written == 1 && "Error packet data to disk");
    m_lastPacketSizeWritten += written * packet.dataSize;
    
    // Increment out counf of frames recorded
    m_framenum++;
}

} // namespace vision
} // namespace ram

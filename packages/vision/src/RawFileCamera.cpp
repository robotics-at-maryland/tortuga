/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/RawFileCamera.h
 */

// STD Includes
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

// Library Includes

// Project Includes
#include "vision/include/RawFileCamera.h"
#include "vision/include/RawFileRecorder.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

RawFileCamera::RawFileCamera(std::string filename) :
    m_width(0),
    m_height(0),
    m_fps(0),
    m_duration(0),
    m_currentTime(0),
    m_currentFrame(0),
    m_dataBuffer(0),
    m_file(0),
    m_fileSize(0)
{
    // Open up the file
    m_file = open(filename.c_str(), O_RDONLY);
    assert(m_file != -1 && "could not open file");
    
    // Read in the header
    RawFileRecorder::Header header;
    size_t countRead = read(m_file, &header, sizeof(RawFileRecorder::Header));
    assert(countRead == sizeof(RawFileRecorder::Header) && "Error reading");
    
    // Verify the magic number
    assert(header.magicNumber == RawFileRecorder::MAGIC_NUMBER
           && "Invalid RawFile magic number");

    // Set up basic parameters
    m_width = header.width;
    m_height = header.height;
    m_fps = header.framerate;
    
    // Determine File Length
    m_fileSize = lseek(m_file, 0, SEEK_END);
    assert(m_fileSize > 0 -1 && "error reading size");
    int ret = lseek(m_file, sizeof(RawFileRecorder::Header), SEEK_SET);
    assert(ret > 0 && "error seeking");

    // Use the raw lenght to calculate duration
    size_t packetCount =
        (m_fileSize - sizeof(RawFileRecorder::Header)) / header.packetSize;
    m_duration = packetCount / m_fps;

    // Allocate picture buffer
    m_dataBuffer = new unsigned char[header.packetSize];
    m_dataBufferSize = header.packetSize; // Includes data and structure
}

RawFileCamera::~RawFileCamera()
{
    // Have to stop background capture before we release the capture!
    cleanup();
    
    //fclose(m_file);
    close(m_file);
    delete m_dataBuffer;
}

void RawFileCamera::update(double timestep)
{
    // Grab the next frame
    readNextFrame();

    // Copy image to public side of the interface
    Image* newImage = new OpenCVImage(m_dataBuffer, width(),
                                      height(), false);

    // Notfiy everyone that the image is uploaded
    capturedImage(newImage);

    // Remove the temp image
    delete newImage;
}

    
void RawFileCamera::readNextFrame(bool hurryUp)
{
    // Quit early if we are already at the end
    if (m_fileSize == lseek(m_file, 0, SEEK_CUR))
        return;
    
    // Read in the packet header
    RawFileRecorder::Packet packet;
    int readCount = read(m_file, &packet, sizeof(RawFileRecorder::Packet));
    assert(readCount == sizeof(RawFileRecorder::Packet) && "Error reading");
    
    // Update the frame counter
    m_currentFrame = packet.framenum;
    
    // Compute the new current time
    m_currentTime = m_currentFrame / m_fps;

    // Resize the picture buffer if needed
    if ((packet.size + sizeof(RawFileRecorder::Packet)) > m_dataBufferSize)
    {
        delete m_dataBuffer;
        m_dataBufferSize = packet.size + sizeof(RawFileRecorder::Packet);
        m_dataBuffer = new unsigned char[m_dataBufferSize];
    }
    
    if (hurryUp)
    {
        // Hurrying up, don't read the new data
        int ret = lseek(m_file, packet.size, SEEK_CUR);
        assert(ret != 0 && "Error seeking in file");
    }
    else
    {
        int dataToRead = (int)packet.size;
        unsigned char* bufferPos = m_dataBuffer;
        
        while (dataToRead > 0)
        {
            // Read in raw picture data
            readCount = read(m_file, bufferPos, dataToRead);
            if (readCount == -1)
                perror("Error in reading");
            assert(readCount != -1 && "Error reading");

            // Decrease the amount we have to read and move up in the buffer
            dataToRead -= readCount;
            bufferPos += readCount;

            // If we got the end of the file, drop out of the loop
            if (m_fileSize == lseek(m_file, 0, SEEK_CUR))
                break;
        }
    }
}


size_t RawFileCamera::width()
{
    return m_width;
}
    
size_t RawFileCamera::height()
{
    return m_height;
}

double RawFileCamera::fps()
{
    return m_fps;
}

double RawFileCamera::duration()
{
    return m_duration;
}

void RawFileCamera::seekToTime(double seconds)
{
    int frameNum = (int)(seconds * fps());
    seekTo(frameNum - 1);
    
    // Read frames until we get to the right place
    double lastTime = -1;
    double secondToLastTime = -1;
    do
    {
        secondToLastTime = lastTime;
        lastTime = currentTime();
        readNextFrame(true);
    } while ((currentTime() < seconds) && (lastTime != secondToLastTime));
}

double RawFileCamera::currentTime()
{
    return m_currentTime;
}
    

void RawFileCamera::seekTo(int frame)
{
    /// TODO: make this a slighty smarter seeker
    if (frame < 0)
        frame = 0;
    
    int fileOffset = sizeof(RawFileRecorder::Header) +
        (m_dataBufferSize * frame);
    
    int ret = lseek(m_file, fileOffset, SEEK_SET);
    if (ret != fileOffset)
    {
        printf("Trying to seek to frame# %d, at offset: %d\n", frame,
               fileOffset);
        perror("Error seeking");
    }
    assert(ret == fileOffset && "Error seeking in file");
}

} // namespace vision
} // namespace ram

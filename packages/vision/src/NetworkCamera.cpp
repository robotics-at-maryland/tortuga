/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Joseph Lisee
 * File:  packages/vision/include/NetworkCamera.cpp
 */

// STD Includes
#include <cassert>
#include <cstdlib>
#include <cerrno>
#include <iostream>

// System Includes
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> // gethosebyname
#include <netinet/in.h>

// Project includes
#include "vision/include/NetworkCamera.h"
#include "vision/include/NetworkRecorder.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

NetworkCamera::NetworkCamera(std::string hostname, boost::uint16_t port) :
    m_addr(0),
    m_sockfd(0),
    m_buffer(0),
    m_bufferSize(0),
    m_width(0),
    m_height(0),
    m_fps(0)
{
    // Setup address
    struct hostent* he = gethostbyname(hostname.c_str());
    if (!he)
    {  
        perror("gethostbyname");
        assert(false && "gethostbyname");
    }

    m_addr = (struct sockaddr_in*)calloc(1, sizeof(*m_addr));
    m_addr->sin_family = AF_INET;    // host byte order
    m_addr->sin_port = htons(port);  // short, network byte order
    m_addr->sin_addr = *((struct in_addr *)he->h_addr);
    
    // Create socket
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
        perror("socket");
        assert(false && "socket");
    }

    int ret = connect(m_sockfd, (struct sockaddr*)m_addr,
                      sizeof(struct sockaddr));
    if (ret < 0)
    {
        perror("connect");
        assert(false && "connect");
    }

    
    // Read first no data packet to get width and height
    ImagePacketHeader header;
    readPacketHeader(&header);
    m_width = header.width;
    m_height = header.height;
    m_fps = 30;
}

NetworkCamera::~NetworkCamera()
{
    // Have to stop background capture before we release the capture!
    cleanup();
    close(m_sockfd);
    free(m_addr);
    free(m_buffer);
}

void NetworkCamera::update(double timestep)
{
    // Read header
    ImagePacketHeader header;
    readPacketHeader(&header);

    // TODO: Don't ignore this return value
    m_width = header.width;
    m_height = header.height;
    m_fps = 30;

    // Make buffer fit incomming image size
    if (header.dataSize && (header.dataSize != m_bufferSize))
    {
        m_bufferSize = header.dataSize;
        if (m_buffer)
            m_buffer = (unsigned char*)realloc(m_buffer, m_bufferSize);
        else
            m_buffer = (unsigned char*)malloc(m_bufferSize);
    }

    // Read image off the wire
    recieve(m_buffer, header.dataSize);

    // Create a temp image which doens't own the buffer, and then call capture
    // with it
    if (header.dataSize)
    {
        OpenCVImage newImage(m_buffer, header.width, header.height, false);
        capturedImage(&newImage);
    }
}

size_t NetworkCamera::width()
{    
    return m_width;
}

size_t NetworkCamera::height()
{
    return m_height;
}

size_t NetworkCamera::fps()
{
    return m_fps;
}

void NetworkCamera::readPacketHeader(ImagePacketHeader* packetHeader)
{
    recieve(packetHeader, sizeof(ImagePacketHeader));
    
    // Transform to local order
    packetHeader->width = ntohs(packetHeader->width);
    packetHeader->height = ntohs(packetHeader->height);
    packetHeader->dataSize = ntohl(packetHeader->dataSize);
}

void NetworkCamera::recieve(void* buf, size_t len)
{
    // Clear the error flag and read the bytes
    errno = 0;

    // Loop until we have all the data off the socket
    while ((errno == 0) && (len != 0u))
    {
        size_t bytes = recv(m_sockfd, buf, len, 0);

        if ((((size_t)-1) == bytes) || (errno != 0))
        {
            perror("recv");
            assert(false && "recv");
        }

        // Decrement current number of bytes to read
        len -= bytes;
        // Remember to move forward are data pointer
        buf = ((char*)buf) + bytes;
    }
}
    
} // namespace vision
} // namespace ram

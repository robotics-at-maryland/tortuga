/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/NetworkRecorder.cpp
 */

// STD Includes
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <iostream>

// System Includes
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

// Project Includes
#include "vision/include/NetworkRecorder.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"

// how many pending connections queue will hold
static const int BACKLOG = 10;

namespace ram {
namespace vision {

NetworkRecorder::NetworkRecorder(Camera* camera,
                                 Recorder::RecordingPolicy policy,
                                 boost::uint16_t port, int policyArg) :
    Recorder(camera, policy, policyArg),
    m_port(port),
    m_listenSocket(-1),
    m_currentSocket(-1),
    m_addr(0),
    m_currentAddr(0)
{
    m_packet.width = htons(static_cast<boost::uint16_t>(camera->width()));
    m_packet.height = htons(static_cast<boost::uint16_t>(camera->height()));
    m_packet.dataSize = 0;
    
    // Setup network
    setupListenSocket();
    
    // Run update, will wait for connections, then send images over the network
    background(-1);
}

NetworkRecorder::~NetworkRecorder()
{
    // Stop the background thread, events, and network connections
    cleanUp();
}

void NetworkRecorder::update(double timeSinceLastUpdate)
{
    // If we don't have a current scket to listen on, then make one
    if (m_listenSocket < 0)
        setupListenSocket();
    
    // If we don't have a current connection, make one
    if (m_currentSocket < 0)
    {
        // Wait for a connection (0.1 second timeout)
        m_currentSocket = acceptConnection(0.1);

        if (-1 != m_currentSocket)
        {
            // Send a no data packet so the camera knows the image size
            m_packet.dataSize = 0;
            sendData(&m_packet, sizeof(ImagePacketHeader));
        }
    }

    // Standard update (calls recordFrame)
    Recorder::update(timeSinceLastUpdate);
}
    
void NetworkRecorder::unbackground(bool join)
{
    // Close sockets (will force update function to stop)
    if (m_listenSocket >= 0)
    {
        close(m_listenSocket);
        m_listenSocket = -1;
    }
    if (m_currentSocket >= 0)
    {
        close(m_currentSocket);
        m_listenSocket = -1;
    }

    free(m_addr);
    m_addr = 0;

    free(m_currentAddr);
    m_currentAddr = 0;

    // Shut down background threads
    Recorder::unbackground(join);
}

void NetworkRecorder::recordFrame(Image* image)
{
    size_t width = image->getWidth();
    size_t height = image->getHeight();
    size_t dataSize = width * height * 3;

    // Create Network order packet
    m_packet.width = htons(static_cast<boost::uint16_t>(width));
    m_packet.height = htons(static_cast<boost::uint16_t>(height));
    m_packet.dataSize = htonl(dataSize);

    if (m_currentSocket >= 0)
    {
        // Send header
        if (sendData(&m_packet, sizeof(ImagePacketHeader)))
        {
            // Send image data
            sendData(image->getData(), dataSize);
        }
    }
}

void NetworkRecorder::setupListenSocket()
{
    // Drop out if setup has already been done
    if (m_listenSocket >= 0)
        return;
    
    // Grab socket to listen on
    m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenSocket < 0)
    {
        perror("socket");
        assert(false && "socket error");
    }

    // Setup my address information
    m_addr = (struct sockaddr_in*)calloc(1, sizeof(*m_addr));
    m_addr->sin_family = AF_INET; // host byte order
    m_addr->sin_port = htons(m_port);  // short, network byte order
    m_addr->sin_addr.s_addr = INADDR_ANY; // auto-fill with my IP

    // Bind the socket to the address
    int ret = bind(m_listenSocket,(struct sockaddr *)m_addr,
                   sizeof(struct sockaddr));
    if (ret < 0)
    {
        perror("bind");
        assert(false && "bind error");
    }

    // Prepare socket for to accept incomming connections
    ret = listen(m_listenSocket, BACKLOG);
    if (ret < 0)
    {
        perror("listen");
        assert(false && "listen error");
    }
}

bool NetworkRecorder::sendData(void* buf, size_t len)
{
    if (send(m_currentSocket, buf, len, 0) < 0)
    {
        perror("send");
        
        // Sending data failed, close connection
        close(m_currentSocket);
        m_currentSocket = -1;
        
        return false;
    }

    return true;
}

int NetworkRecorder::acceptConnection(double timeout)
{
    m_currentAddr = (struct sockaddr_in*)calloc(sizeof(*m_currentAddr), 1);
    socklen_t sin_size = sizeof(struct sockaddr_in);
    int newfd = 0;

    // Use select to get a timeout type accept call
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_listenSocket, &fds);

    // Build timeout from fractional number
    double intpart;
    struct timeval timevalTimeout = {0};
    timevalTimeout.tv_usec = (long)(modf(timeout, &intpart) * 1000000);
    timevalTimeout.tv_sec = (long)intpart;

    if (1 == select(m_listenSocket + 1, &fds, NULL, NULL, &timevalTimeout))
    {
        newfd = accept(m_listenSocket,
                       (struct sockaddr*)m_currentAddr,
                       &sin_size);
            
        // Error with connection, just try again
        if (newfd < 0)
        {
            perror("accept");
        }
        else
        {
            return newfd;
        }
    }

    return -1;
}
    
} // namespace vision
} // namespace ram

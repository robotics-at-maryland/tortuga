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
#ifdef RAM_POSIX
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h> // gethostbyname
	#include <netinet/in.h>
#else
	#include <winsock2.h>
#endif

// Project includes
#include "vision/include/NetworkCamera.h"
#include "vision/include/NetworkRecorder.h"
#include "vision/include/OpenCVImage.h"

#define RAM_NETWORK_COMPRESSION

#ifdef RAM_NETWORK_COMPRESSION
#include "vision/include/quicklz.h"
#endif

#ifdef RAM_WINDOWS
#define close closesocket
#endif

namespace ram {
namespace vision {

NetworkCamera::NetworkCamera(std::string hostname, boost::uint16_t port) :
    m_addr(0),
    m_sockfd(0),
    m_compressedBuffer(0),
    m_imageBuffer(0),
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

#ifdef RAM_WINDOWS
	WSADATA wsaData;
	WORD version;
	int error;
	version = MAKEWORD( 2, 0 );
	error = WSAStartup( version, &wsaData );
#endif

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
    ImagePacketHeader header = {0, 0, 0};
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
#ifdef RAM_WINDOWS
    WSACleanup();
#endif

    free(m_addr);
    free(m_compressedBuffer);
    free(m_imageBuffer);
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
    // Need to fix buffer sizes better
#ifdef RAM_NETWORK_COMPRESSION
    size_t dataSize = m_width * m_height * 3;
#else
    size_t dataSize = header.dataSize;
#endif
    if (dataSize && (dataSize != m_bufferSize))
    {
        m_bufferSize = dataSize;
        if (m_compressedBuffer)
        {
            m_compressedBuffer = (unsigned char*)realloc(m_compressedBuffer,
                                                         m_bufferSize);
            m_imageBuffer = (unsigned char*)realloc(m_imageBuffer,
                                                    m_bufferSize);
        }
        else
        {
            m_compressedBuffer = (unsigned char*)malloc(m_bufferSize);
            m_imageBuffer = (unsigned char*)malloc(m_bufferSize);
        }
    }

    // Read image off the wire
    recieve(m_compressedBuffer, header.dataSize);

    // Create a temp image which doens't own the buffer, and then call capture
    // with it
    if (header.dataSize)
    {
#ifdef RAM_NETWORK_COMPRESSION
        char scratch[QLZ_SCRATCH_DECOMPRESS];
        /*size_t newSize = */qlz_decompress((char*)m_compressedBuffer,
                                            (void*)m_imageBuffer, scratch);
        OpenCVImage newImage(m_imageBuffer, header.width, header.height,
                             false);
#else
        OpenCVImage newImage(m_buffer, header.width, header.height, false);
#endif

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
        size_t bytes = recv(m_sockfd, (char*)buf, len, 0);

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

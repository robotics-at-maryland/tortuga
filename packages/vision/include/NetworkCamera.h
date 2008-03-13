/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/NetworkCamera.h
 */

#ifndef RAM_VISION_NETWORKCAMERA_H_02_25_2008
#define RAM_VISION_NETWORKCAMERA_H_02_25_2008

// STD Includes
#include <string>

// Library Includes
#include <boost/cstdint.hpp>
#include <boost/thread/mutex.hpp>

// Compression
#include "quicklz.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"
#include "vision/include/Export.h"

namespace ram {
namespace vision {

struct ImagePacketHeader;
    
class RAM_EXPORT NetworkCamera : public Camera
{
public:
    /** Creates a camera which read dad from the given host the given port */
    NetworkCamera(std::string hostname, boost::uint16_t port);
    
    ~NetworkCamera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);
	
    virtual size_t width();
    
    virtual size_t height();

    virtual size_t fps();

private:
    /** Reads a packet off the socket, and transforms to host order
     *
     *  @return   false if the connection is no longer valid
     */
    void readPacketHeader(ImagePacketHeader* packetHeader);
    
    /** */
    void recieve(void* bug, size_t len);

    /** Protects access to the socket and addr data */                          
    boost::mutex m_networkMutex;
    
    /** Address to connect to */
    struct sockaddr_in* m_addr;
    
    /** The socket we are reading data from */
    int m_sockfd;

    /** The buffer where we copy the image network data */
    unsigned char* m_compressedBuffer;
    /** The buffer where the decompressed network data is stored */
    unsigned char* m_imageBuffer;
    
    size_t m_bufferSize;

    /** Protects access to m_width, m_height, and m_fps variables */            
    boost::mutex m_specsMutex;   
    
    size_t m_width;
    size_t m_height;
    size_t m_fps;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_NETWORKCAMERA_H_02_25_2008

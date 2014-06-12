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
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

// Compression
#include "quicklz.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT NetworkCamera : public Camera
{
public:
    /** Creates a camera which read dad from the given host the given port */
    NetworkCamera(std::string hostname, boost::uint16_t port);
    
    virtual ~NetworkCamera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);

    virtual size_t width();
    
    virtual size_t height();

    virtual double fps();

    virtual double duration();

    virtual void seekToTime(double seconds);

    virtual double currentTime();

private:
    boost::asio::io_service io_service;

    std::string m_hostname;
    std::string m_port;
    boost::asio::ip::tcp::endpoint m_endpoint;

    boost::mutex m_diagLock;
    size_t m_width;
    size_t m_height;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_NETWORKCAMERA_H_02_25_2008

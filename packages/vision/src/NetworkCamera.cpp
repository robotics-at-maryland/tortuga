/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Joseph Lisee
 * File:  packages/vision/include/NetworkCamera.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <highgui.h>

// Project includes
#include "vision/include/NetworkCamera.h"
#include "vision/include/NetworkRecorder.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

NetworkCamera::NetworkCamera(std::string hostname, boost::uint16_t port)
    : m_hostname(hostname)
    , m_port(boost::lexical_cast<std::string>(port))
    , m_width(320)
    , m_height(240)
{
}

NetworkCamera::~NetworkCamera()
{
    // Have to stop background capture before we release the capture!
    cleanup();
}

void NetworkCamera::update(double timestep)
{
    using namespace boost::asio::ip;

    tcp::socket socket(io_service);

    tcp::endpoint none;
    if (m_endpoint == none) {
        // New connection
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(m_hostname, m_port);

        tcp::resolver::iterator iter = resolver.resolve(query);
        tcp::resolver::iterator end;
        boost::system::error_code error = boost::asio::error::host_not_found;
        do {
            socket.close();
            socket.connect(*iter, error);
        } while (error && ++iter != end);

        if (error)
            return; // could not connect to host
        else {
            // Save endpoint for next time
            m_endpoint = *iter;
        }
    } else {
        boost::system::error_code error;
        socket.connect(m_endpoint, error);

        if (error) {
            // host has succeeded in the past, but now fails. reset.
            m_endpoint = none;
            return;
        }
    }

    // read in the image data
    try {
        size_t packetSize;
        boost::asio::read(socket, boost::asio::buffer(
                              &packetSize, sizeof(size_t)));

        std::vector<unsigned char> buf(packetSize);
        size_t len = boost::asio::read(
            socket, boost::asio::buffer(buf), boost::asio::transfer_all());
        if (len != packetSize) {
            std::cout << "incorrect number of bytes received!" << std::endl;
            return;
        }

        cv::Mat mat = cv::imdecode(cv::Mat(buf), 1);
        IplImage img = (IplImage) mat;
        OpenCVImage newImage(&img, false);

        capturedImage(&newImage);
    } catch (boost::system::system_error &error) {
        // bad error (don't want to crash though, so don't rethrow)
        std::cout << error.what() << std::endl;
    }
}

size_t NetworkCamera::width()
{
    boost::mutex::scoped_lock lock(m_diagLock);
    return m_width;
}

size_t NetworkCamera::height()
{
    boost::mutex::scoped_lock lock(m_diagLock);
    return m_height;
}

double NetworkCamera::fps()
{
    return 0;
}

double NetworkCamera::duration()
{
    return 0;
}
  
void NetworkCamera::seekToTime(double seconds)
{
}

double NetworkCamera::currentTime()
{
    return 0;
}

} // namespace vision
} // namespace ram

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
#include <cstdio>
#include <iostream>
#include <sstream>

// Library Includes
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <highgui.h>

// Project Includes
#include "vision/include/NetworkRecorder.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

#include "core/include/TimeVal.h"

namespace ba = boost::asio;
using namespace boost::asio::ip;

namespace ram {
namespace vision {

NetworkRecorder::NetworkRecorder(Camera* camera,
                                 Recorder::RecordingPolicy policy,
                                 boost::uint16_t port, int policyArg,
                                 int recordWidth, int recordHeight) :
    Recorder(camera, policy, policyArg, recordWidth, recordHeight),
    m_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    m_buffer(new OpenCVImage()) // size will be changed by copyFrom
{
    start_accept();
    m_bthread = new boost::thread(
        boost::bind(&NetworkRecorder::run_service, this));
    background(-1);
}

NetworkRecorder::~NetworkRecorder()
{
    // Stop the background thread, events, and network connections
    cleanUp();
    io_service.stop();
    m_bthread->join();
    delete m_buffer;
}

void NetworkRecorder::start_accept()
{
    Connection::pointer new_connection =
        Connection::create(m_acceptor.io_service());

    m_acceptor.async_accept(
        new_connection->socket(),
        boost::bind(&NetworkRecorder::handle_accept, this,
                    new_connection, boost::asio::placeholders::error));
}

void NetworkRecorder::handle_accept(Connection::pointer new_connection,
                                    const boost::system::error_code& error)
{
    if (!error)
    {
        {
            OpenCVImage image;
            {
                boost::mutex::scoped_lock lock(m_lock);
                image.copyFrom(m_buffer);
                image.setSize(160, 120);
            }

            new_connection->sendImage(&image);
        }

        start_accept();
    }
}

void NetworkRecorder::recordFrame(Image* image)
{
    boost::mutex::scoped_lock lock(m_lock);
    m_buffer->copyFrom(image);
}
    
NetworkRecorder::Connection::pointer
NetworkRecorder::Connection::create(ba::io_service& io)
{
    return NetworkRecorder::Connection::pointer(
        new NetworkRecorder::Connection(io));
}

void NetworkRecorder::Connection::sendImage(Image *img)
{
    std::vector<unsigned char> buf;
    img->setPixelFormat(Image::PF_BGR_8);
    cv::imencode(".jpg", cv::Mat(img->asIplImage()), buf);

    // send compressed data
    size_t len = buf.size();
    ba::write(m_socket, ba::buffer(&len, sizeof(size_t)));
    ba::write(m_socket, ba::buffer(buf), ba::transfer_all());
}

tcp::socket& NetworkRecorder::Connection::socket()
{
    return m_socket;
}

NetworkRecorder::Connection::Connection(ba::io_service& io)
    : m_socket(io)
{
}
    
} // namespace vision
} // namespace ram

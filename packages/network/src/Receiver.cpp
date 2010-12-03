/*
 * Copyright 2010 Occam's Mirror
 * Copyright 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/src/Receiver.cpp
 */

// STD Includes
#include <iostream>
#include <sstream>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "network/include/Receiver.h"

using namespace boost::asio::ip;

namespace ram {
namespace network {

Receiver::Receiver(boost::uint16_t port, size_t bufferSize,
                   boost::function< void (const char*) > handler)
    : m_socket(io_service, udp::endpoint(udp::v4(), port))
    , m_port(port)
    , m_handler(handler)
    , m_bufferSize(bufferSize)
{
    // Allocate buffer
    m_data = new char[m_bufferSize];

    // Start background thread to run io service
    m_bthread = new boost::thread(boost::bind(&Receiver::mainLoop, this));
}

Receiver::~Receiver()
{
    // Stop background thread
    io_service.stop();
    m_bthread->join();

    delete m_bthread;
    delete [] m_data;
}

void Receiver::mainLoop()
{
    start_receive();
    io_service.run();
}

void Receiver::start_receive()
{
    m_socket.async_receive_from(
        boost::asio::buffer(m_data, m_bufferSize), sender_endpoint,
        boost::bind(&Receiver::handle_receive_from, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Receiver::handle_receive_from(const boost::system::error_code& err,
    size_t bytes_recvd)
{
    // Send the raw data to the handler
    if (!err && bytes_recvd > 0) {
        m_handler(m_data);
    }

    // Start new accept
    start_receive();
}

boost::uint16_t Receiver::getPort()
{
    return m_port;
}

} // namespace network
} // namespace ram

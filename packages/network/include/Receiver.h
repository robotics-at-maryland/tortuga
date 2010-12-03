/*
 * Copyright 2010 Robotics at Maryland
 * Copyright 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/include/Receiver.h
 */

#ifndef RAM_NETWORK_RECEIVER_H_10_18_2010
#define RAM_NETWORK_RECEIVER_H_10_18_2010

// STD Includes
#include <string>

// Library Includes
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

// Project Includes
#include "network/include/Common.h"

// Must be included last
#include "network/include/Export.h"

namespace ram {
namespace network {

class RAM_EXPORT Receiver
{
public:
    Receiver(boost::uint16_t port, size_t buffer_size,
             boost::function< void (const char*) > handler);

    ~Receiver();

    /**
     * Gets the port that this receiver is listening on.
     */
    boost::uint16_t getPort();

private:
    void mainLoop();
    void start_receive();
    void handle_receive_from(const boost::system::error_code& err,
                             size_t bytes_recvd);

    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint sender_endpoint;
    boost::uint16_t m_port;
    boost::function< void (const char*) > m_handler;
    boost::thread *m_bthread;

    size_t m_bufferSize;
    char *m_data;
};

} // namespace network
} // namespace ram

#endif // RAM_NETWORK_RECEIVER_H_10_18_2010

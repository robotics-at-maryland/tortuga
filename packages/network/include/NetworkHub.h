/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/include/NetworkHub.h
 */

#ifndef RAM_NETWORK_NETWORKHUB_11_15_2010
#define RAM_NETWORK_NETWORKHUB_11_15_2010

// Library Includes
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// Project Includes
#include "core/include/EventHub.h"
#include "network/include/NetworkPublisher.h"

// Must be included last
#include "network/include/Export.h"

namespace ram {
namespace network {

class RAM_EXPORT NetworkHub : public core::EventHub
{
public:
    /** Normal constructor */
    NetworkHub(std::string name = "NetworkHub",
               std::string host = "localhost",
               uint16_t port = NetworkPublisher::PORT);

    /** Standard subsystem constructor */
    NetworkHub(core::ConfigNode config,
               core::SubsystemList deps = core::SubsystemList());

    virtual ~NetworkHub();

private:
    void sendRequest();
    void daemon();

    std::string m_host;
    uint16_t m_port;

    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket_;
    boost::thread *m_bthread;
    bool m_active;
};

} // namespace network
} // namespace ram

#endif // RAM_NETWORK_NETWORKHUB_11_15_2010

/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/src/NetworkHub.cpp
 */

// STD Includes
#include <sstream>

// Library Includes
#include <boost/archive/text_iarchive.hpp>
#include <boost/lexical_cast.hpp>

// Project Includes
#include "core/include/Event.h"
#include "core/include/SubsystemMaker.h"
#include "network/include/NetworkHub.h"
#include "logging/include/Serialize.h"

#define MAX_LENGTH 256

RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::network::NetworkHub,
                                  NetworkHub);

using namespace boost::asio::ip;

namespace ram {
namespace network {

NetworkHub::NetworkHub(core::EventHubPtr eventHub,
                       std::string name, std::string host, uint16_t port)
    : core::EventHub(name),
      m_host(host),
      m_port(port),
      socket_(io_service, udp::endpoint(udp::v4(), 0)),
      m_bthread(0),
      m_active(true)
{
    sendRequest();
    m_bthread = new boost::thread(boost::bind(&NetworkHub::daemon, this));
}

NetworkHub::NetworkHub(core::ConfigNode config,
                       core::SubsystemList deps)
    : core::EventHub(config, deps),
      socket_(io_service, udp::endpoint(udp::v4(), NetworkPublisher::PORT)),
      m_bthread(0),
      m_active(true)
{
    sendRequest();
    m_bthread = new boost::thread(boost::bind(&NetworkHub::daemon, this));
}

NetworkHub::~NetworkHub()
{
    // Stop background processing thread
    m_active = false;
    m_bthread->join();

    delete m_bthread;
}

void NetworkHub::sendRequest()
{
    // Resolve host name and port number
    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), m_host,
                               boost::lexical_cast<std::string>(m_port));
    udp::resolver::iterator iterator = resolver.resolve(query);

    // Send empty signal to NetworkPublisher to establish connection
    socket_.send_to(boost::asio::buffer((char *) NULL, 0), *iterator);
}

void NetworkHub::daemon()
{
    while (m_active)
    {
        // Block for message
        char reply[MAX_LENGTH];
        udp::endpoint sender_endpoint;
        size_t reply_length = socket_.receive_from(
            boost::asio::buffer(reply, MAX_LENGTH), sender_endpoint);

        // Write message received to archive
        std::stringstream sstream;
        sstream.write(reply, reply_length);

        // Deserialize the received event
        boost::archive::text_iarchive archive(sstream);
        core::EventPtr event = core::EventPtr();
        archive >> event;

        // Publish the deserialized event
        publish(event);
    }
}

} // namespace network
} // namespace ram

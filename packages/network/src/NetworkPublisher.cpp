/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/src/NetworkPublisher.cpp
 */

// STD Includes
#include <sstream>

// Library Includes
#include <boost/bind.hpp>
#include <boost/archive/text_oarchive.hpp>

// Project Includes
#include "network/include/NetworkPublisher.h"
#include "core/include/EventHub.h"
#include "core/include/SubsystemMaker.h"
#include "logging/include/Serialize.h"

// Must be included last
#include "network/include/Export.h"

RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::network::NetworkPublisher,
                                  NetworkPublisher);

using namespace boost::asio::ip;

namespace ram {
namespace network {

const uint16_t NetworkPublisher::PORT = 51346;

NetworkPublisher::NetworkPublisher(core::ConfigNode config,
                                   core::SubsystemList deps) :
    core::Subsystem(config["name"].asString("NetworkPublisher"),
                    core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_eventHub(core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    socket_(io_service, udp::endpoint(udp::v4(), config["port"].asInt(PORT))),
    m_bthread(0)
{
    assert(m_eventHub && "Need an EventHub");
    m_eventHub->subscribeToAll(boost::bind(&NetworkPublisher::handleEvent, this, _1));
}

NetworkPublisher::~NetworkPublisher()
{
}

void NetworkPublisher::update(double timeSinceLastUpdate)
{
}

void NetworkPublisher::background(int interval)
{
    startReceive();
    m_bthread = new boost::thread(
        boost::bind(&NetworkPublisher::serviceRequests, this));
}

void NetworkPublisher::unbackground(bool join)
{
    io_service.stop();
    m_bthread->join();

    delete m_bthread;
    m_bthread = 0;
}

bool NetworkPublisher::backgrounded()
{
    return m_bthread != NULL;
}

void NetworkPublisher::startReceive()
{
    socket_.async_receive_from(
        boost::asio::buffer((char *) NULL, 0), sender_endpoint,
        boost::bind(&NetworkPublisher::handleReceiveFrom, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void NetworkPublisher::handleReceiveFrom(const boost::system::error_code& err,
                                   size_t bytes_recvd)
{
    if (!err) {
        // Add the sender's endpoint to the list of endpoints if no error
        boost::mutex::scoped_lock lock(m_mutex);
        m_endpoints.insert(sender_endpoint);
    }

    // Receive a new connection
    startReceive();
}

void NetworkPublisher::handleSend(const boost::system::error_code& err,
                            size_t bytes_sent)
{
}

void NetworkPublisher::serviceRequests()
{
    io_service.run();
}

void NetworkPublisher::handleEvent(core::EventPtr event)
{
    // Serialize event to archive
    std::stringstream sstream;
    bool writeSuccessful = false;

    {
        boost::archive::text_oarchive archive(sstream);
        writeSuccessful = logging::writeEvent(event, archive);
    }

    // Send serialized data to all registered endpoints
    if ( writeSuccessful )
    {
        boost::mutex::scoped_lock lock(m_mutex);
        BOOST_FOREACH(udp::endpoint recipient, m_endpoints)
        {
            socket_.async_send_to(
                boost::asio::buffer(sstream.str(), sstream.str().size() + 1),
                recipient,
                boost::bind(&NetworkPublisher::handleSend, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        }
    }
}

} // namespace network
} // namespace ram

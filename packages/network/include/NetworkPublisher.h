/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/include/NetworkPublisher.h
 */

#ifndef RAM_NETWORK_NETWORKHUB_11_11_2010
#define RAM_NETWORK_NETWORKHUB_11_11_2010

// STD Includes
#include <set>

// Library Includes
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/Updatable.h"
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "core/include/EventHub.h"

namespace ram {
namespace network {

class RAM_EXPORT NetworkPublisher :
        public core::Subsystem,
        public core::Updatable
{
  public:
    NetworkPublisher(core::ConfigNode config,
               core::SubsystemList deps = core::SubsystemList());

    virtual ~NetworkPublisher();

    /** Does nothing (reads are done in a background process) */
    virtual void update(double timeSinceLastUpdate);

    virtual void setPriority(core::IUpdatable::Priority priority) {
        Updatable::setPriority(priority);
    }
    
    virtual core::IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    virtual void setAffinity(size_t affinity) {
        Updatable::setAffinity(affinity);
    }
    
    virtual int getAffinity() {
        return Updatable::getAffinity();
    }
    
    virtual void background(int interval);

    virtual void unbackground(bool join = false);

    virtual bool backgrounded();

  private:
    void startReceive();

    void handleReceiveFrom(const boost::system::error_code& err,
                           size_t bytes_recvd);

    void handleSend(const boost::system::error_code& err,
                    size_t bytes_sent);

    void serviceRequests();

    void handleEvent(core::EventPtr event);

    core::EventHubPtr m_eventHub;

    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket_;
    boost::thread *m_bthread;

    boost::asio::ip::udp::endpoint sender_endpoint;
    std::set<boost::asio::ip::udp::endpoint> m_endpoints;
    boost::mutex m_mutex;

    std::set<std::string> m_unconvertableTypes;
};

} // namespace network
} // namespace ram

#endif // RAM_NETWORK_NETWORKHUB_11_11_2010

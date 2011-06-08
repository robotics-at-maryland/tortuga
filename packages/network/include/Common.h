/*
 * Copyright 2010 Robotics at Maryland
 * Copyright 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/include/Common.h
 */

#ifndef RAM_NETWORK_COMMON_H_10_18_2010
#define RAM_NETWORK_COMMON_H_10_18_2010

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace network {

class Receiver;

class RemoteController;
typedef boost::shared_ptr<RemoteController> RemoteControllerPtr;

class NetworkPublisher;
typedef boost::shared_ptr<NetworkPublisher> NetworkPublisherPtr;

class NetworkHub;
typedef boost::shared_ptr<NetworkHub> NetworkHubPtr;

class NetworkAdapter;
typedef boost::shared_ptr<NetworkAdapter> NetworkAdapterPtr;

} // namespace network
} // namespace ram

#endif // RAM_NETWORK_COMMON_H_10_18_2010

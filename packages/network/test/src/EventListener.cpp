/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/logging/test/src/EventListener.cpp
 */

// STD Includes
#include <stdint.h>
#include <time.h>
#include <iostream>

// Library Includes
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

// Project Includes
#include "core/include/Event.h"
#include "core/include/EventHub.h"
#include "network/include/Common.h"
#include "network/include/NetworkHub.h"

using namespace ram;

bool active = true;

void sigHandler(int num)
{
    active = false;
}

void handleEvent(core::EventPtr event)
{
    std::cout << "Event\ntype: " << event->type
              << "\ntimeStamp: " << event->timeStamp;
    if (event->sender) {
        std::cout << "\nsender: " << event->sender->getPublisherName();
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return 1;
    }

    network::NetworkHubPtr eventHub(
        new network::NetworkHub(core::EventHubPtr(), "NetworkHub", argv[1],
                                boost::lexical_cast<uint16_t>(argv[2])));
    eventHub->subscribeToAll(boost::bind(&handleEvent, _1));

    while (active) {
        usleep(100);
    }
    return 0;
}

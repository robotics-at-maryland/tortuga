/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/network/src/NetworkController.cpp
 */

// Networking includes
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

// Project Includes
#include "network/include/NetworkController.h"
#include "network/include/Events.h"
#include "core/include/EventHub.h"
#include "core/include/SubsystemMaker.h"
#include "estimation/include/IStateEstimator.h"
#include "math/include/Helpers.h"
#include "math/include/Quaternion.h"
#include "math/include/Events.h"

// Should be in a shared header somewhere
#define MYPORT 9219 /*YOUR PORT HERE*/

#define CMD_NOTHING       0

#define CMD_TURNLEFT      1
#define CMD_TURNRIGHT     2

#define CMD_ASCEND        3
#define CMD_DESCEND       4

#define CMD_INCSPEED      5
#define CMD_DECSPEED      6

#define CMD_ZEROSPEED     7
#define CMD_EMERGSTOP     8

#define CMD_SETSPEED      9
#define CMD_TSETSPEED    11

#define CMD_ANGLEYAW     10

#define CMD_SETSPEED      9

#define CMD_ANGLEYAW     10
#define CMD_ANGLEPITCH   12
#define CMD_ANGLEROLL    13

#define CMD_FIRE_TORPEDO 14
#define CMD_DROP_MARKER  15

RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::network::NetworkController,
                                  NetworkController);

namespace ram {
namespace network {

NetworkController::NetworkController(core::ConfigNode config,
                                   core::SubsystemList deps) :
    core::Subsystem(config["name"].asString(),
                    core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_port(config["port"].asInt(MYPORT)),
    m_receiver(0)
{
    enable();
}

NetworkController::~NetworkController()
{
    disable();
}

void NetworkController::enable()
{
    /* Safe to call multiple times */
    if (!m_receiver) {
        m_receiver = new Receiver(m_port, 2, boost::bind(&NetworkController::accept, this, _1));
    }
}

void NetworkController::disable()
{
    /* Safe to call multiple times */
    if (m_receiver) {
        delete m_receiver;
        m_receiver = NULL;
    }
}

void NetworkController::update(double)
{
}

void NetworkController::background(int interval)
{
}

void NetworkController::unbackground(bool join)
{
}

void NetworkController::accept(const char* msg)
{
    processMessage(msg[0], msg[1]);
}

bool NetworkController::processMessage(unsigned char cmd, signed char param)
{
    switch(cmd)
    {
    case CMD_EMERGSTOP:
    {
        // Return false to stop the main network loop
        core::EventPtr event(new core::Event());
        publish(EventType::EMERGENCY_STOP, event);
        return false;
        break;
    }

    case CMD_TURNLEFT:
    {
        core::EventPtr event(new core::Event());
        publish(EventType::YAW_LEFT, event);
        break;
    }

    case CMD_TURNRIGHT:
    {
        core::EventPtr event(new core::Event());
        publish(EventType::YAW_RIGHT, event);
        break;
    }

    case CMD_INCSPEED:
    {
        // Command is inherently dangerous, disabled
        break;
    }

    case CMD_DECSPEED:
    {
        // Command is inherently dangerous, disabled
        break;
    }

    case CMD_DESCEND:
    {
        core::EventPtr event(new core::Event());
        publish(EventType::DESCEND, event);
        break;
    }

    case CMD_ASCEND:
    {
        core::EventPtr event(new core::Event());
        publish(EventType::ASCEND, event);
        break;
    }

    case CMD_ZEROSPEED:
    {
        // Ignoring this command for now
        break;
    }

    case CMD_SETSPEED:
    {
        math::NumericEventPtr event(new math::NumericEvent());
        event->number = param;
        publish(EventType::SETSPEED, event);
        break;
    }

    case CMD_TSETSPEED:
    {
        math::NumericEventPtr event(new math::NumericEvent());
        event->number = param;
        publish(EventType::TSETSPEED, event);
        break;
    }
    case CMD_NOTHING:
    {
        // Ignore, just sent to keep the connection alive
        break;
    }

    case CMD_ANGLEYAW:
    {
        math::NumericEventPtr event(new math::NumericEvent());
        event->number = param;
        publish(EventType::ANGLEYAW, event);
        break;
    }

    case CMD_ANGLEPITCH:
    {
        math::NumericEventPtr event(new math::NumericEvent());
        event->number = param;
        publish(EventType::ANGLEPITCH, event);
        break;
    }

    case CMD_ANGLEROLL:
    {
        math::NumericEventPtr event(new math::NumericEvent());
        event->number = param;
        publish(EventType::ANGLEROLL, event);
        break;
    }

    case CMD_DROP_MARKER:
    {
        core::EventPtr event(new core::Event());
        publish(EventType::FIRE_MARKER_DROPPER, event);
    }

    case CMD_FIRE_TORPEDO:
    {
        core::EventPtr event(new core::Event());
        publish(EventType::FIRE_TORPEDO_LAUNCHER, event);
    }

    default:
    {
    }

    }
    
    // Return true to keep running
    return true;
}

} // namespace network
} // namespace ram

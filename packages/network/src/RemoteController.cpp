/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/network/src/RemoveController.cpp
 */

// Networking includes
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

// Project Includes
#include "network/include/RemoteController.h"
#include "control/include/IController.h"
#include "core/include/EventHub.h"
#include "core/include/SubsystemMaker.h"
#include "estimation/include/IStateEstimator.h"
#include "math/include/Helpers.h"
#include "math/include/Quaternion.h"

// TODO: move these to configured data memebers of the class
// Setting for the hand control inputs
#define MAX_DEPTH 10
#define MIN_DEPTH 0

#define DEPTH_ENC 0.25
#define TURN_ENC 10

#define YAW_GAIN 100.0
#define PITCH_GAIN 100.0
#define ROLL_GAIN 100.0

#define MIN_SPEED -5
#define MAX_SPEED 5
#define SPEED_ENC 1

// Should be in a shared header somewhere
#define MYPORT 9219 /*YOUR PORT HERE*/

#define CMD_NOTHING     0

#define CMD_TURNLEFT    1
#define CMD_TURNRIGHT   2

#define CMD_ASCEND      3
#define CMD_DESCEND     4

#define CMD_INCSPEED    5
#define CMD_DECSPEED    6

#define CMD_ZEROSPEED   7
#define CMD_EMERGSTOP   8

#define CMD_SETSPEED    9
#define CMD_TSETSPEED 11

#define CMD_ANGLEYAW	10

#define CMD_SETSPEED    9

#define CMD_ANGLEYAW 10
#define CMD_ANGLEPITCH 12
#define CMD_ANGLEROLL 13



RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::network::RemoteController,
                                  RemoteController);

namespace ram {
namespace network {

RemoteController::RemoteController(core::ConfigNode config,
                                   core::SubsystemList deps) :
    core::Subsystem(config["name"].asString(),
                    core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_port(config["port"].asInt(MYPORT)),
    m_controller(core::Subsystem::getSubsystemOfType<control::IController>(deps)),
    m_stateEstimator(core::Subsystem::getSubsystemOfType<estimation::IStateEstimator>(deps)),
    m_receiver(0)
{
    //assert(m_controller.get() != 0 && "Did not get controller");
    assert(m_stateEstimator.get() != 0 && "Did not get estimator");
    m_velocity_b = math::Vector2::ZERO;
    m_maxDepth = config["maxDepth"].asDouble(MAX_DEPTH);
    m_minDepth = config["minDepth"].asDouble(MIN_DEPTH);
    m_depthEnc = config["depthEnc"].asDouble(DEPTH_ENC);
    m_turnEnc = config["turnEnc"].asDouble(TURN_ENC);
    m_minSpeed = config["minSpeed"].asDouble(MIN_SPEED);
    m_maxSpeed = config["maxSpeed"].asDouble(MAX_SPEED);
    m_speedEnc = config["speedEnc"].asDouble(SPEED_ENC);
    m_yawGain = config["yawGain"].asDouble(YAW_GAIN);
    m_pitchGain = config["pitchGain"].asDouble(PITCH_GAIN);
    m_rollGain = config["rollGain"].asDouble(ROLL_GAIN);
}

RemoteController::~RemoteController()
{
    disable();
}

void RemoteController::enable()
{
    /* Safe to call multiple times */
    if (!m_receiver) {
        m_receiver = new Receiver(m_port, 2, boost::bind(&RemoteController::accept, this, _1));
    }
}

void RemoteController::disable()
{
    /* Safe to call multiple times */
    if (m_receiver) {
        delete m_receiver;
        m_receiver = NULL;
    }
}

void RemoteController::update(double)
{
    // // Wait for command packet
    // signed char buf[2];
    // int len = recvfrom(m_sockfd, buf, 2, 0, NULL, NULL);
    // if(len != 2)
    // {
    //     // If error from network drop out of receive loop
    //     printf("Error reading from network\n");
    // }
    // //printf("Got message\n");

    // if (backgrounded())
    // {
    //     // Break out commands and parameters
    //     unsigned char cmd = buf[0];
    //     signed char param = buf[1];
    //     //printf("Procssing");
    //     // Process Packet (If quit message drop out of loop, stop running)
    //     processMessage(cmd, param);
    // }
}

void RemoteController::background(int interval)
{
    enable();
}

void RemoteController::unbackground(bool join)
{
    disable();
}

void RemoteController::accept(const char* msg)
{
    processMessage(msg[0], msg[1]);
}

bool RemoteController::processMessage(unsigned char cmd, signed char param)
{
    switch(cmd)
    {
    case CMD_EMERGSTOP:
    {
        //printf("Emergency stop received\n");
        // Return false to stop the main network loop
        return false;
        break;
    }

    case CMD_TURNLEFT:
    {
        //printf("Yaw left\n");
        m_controller->yawVehicle(m_turnEnc, 0);
        break;
    }

    case CMD_TURNRIGHT:
    {
        //printf("Yaw right\n");
        m_controller->yawVehicle(-m_turnEnc, 0);
        break;
    }

    case CMD_INCSPEED:
    {
        // get the current estimated position
        math::Vector2 position = m_stateEstimator->getEstimatedPosition();
        
        // get the current desired yaw
        double yaw = m_stateEstimator->getEstimatedOrientation().getYaw().valueRadians();

        // define a vector for the velocity increase (body frame)
        math::Vector2 vInc_b(m_speedEnc, 0);
        math::Vector2 newVelocity_b = m_velocity_b + vInc_b;
        math::Vector2 newVelocity_n;

        if(newVelocity_b[0] < m_maxSpeed && 
           newVelocity_b[1] < m_maxSpeed)
        {
            newVelocity_n = math::nRb(yaw) * newVelocity_b;
            m_velocity_b = newVelocity_b;
        }
        else
            newVelocity_n = math::nRb(yaw) * m_velocity_b;

        m_controller->translate(position, newVelocity_n);

        break;
    }

    case CMD_DECSPEED:
    {
        // get the current estimated position
        math::Vector2 position = m_stateEstimator->getEstimatedPosition();
        
        // get the current desired yaw
        double yaw = m_stateEstimator->getEstimatedOrientation().getYaw().valueRadians();

        // define a vector for the velocity increase (body frame)
        math::Vector2 vInc_b(m_speedEnc, 0);
        math::Vector2 newVelocity_b = m_velocity_b - vInc_b;
        math::Vector2 newVelocity_n;

        if(newVelocity_b[0] > m_minSpeed && 
           newVelocity_b[1] > m_minSpeed)
        {
            newVelocity_n = math::nRb(yaw) * newVelocity_b;
            m_velocity_b = newVelocity_b;
        }
        else
            newVelocity_n = math::nRb(yaw) * m_velocity_b;

        m_controller->translate(position, newVelocity_n);

        break;
    }

    case CMD_DESCEND:
    {
        double depth = m_controller->getDesiredDepth();
        if(depth < m_maxDepth)
            m_controller->changeDepth(depth + m_depthEnc, 0);

        //printf("NEW DEPTH: %f\n", m_controller->getDepth());
        break;
    }

    case CMD_ASCEND:
    {
        double depth = m_controller->getDesiredDepth();
        if(depth < m_maxDepth)
            m_controller->changeDepth(depth - m_depthEnc, 0);

        //printf("NEW DEPTH: %f\n", m_controller->getDepth());
        break;
    }

    case CMD_ZEROSPEED:
    {
        m_controller->holdCurrentPosition();
        //printf("\nNEW SPEED:  %f\n", m_controller->getSpeed());
        break;
    }

    case CMD_SETSPEED:
    {
        // get the current estimated position
        math::Vector2 position = m_stateEstimator->getEstimatedPosition();
        
        // get the current desired yaw
        math::Quaternion orientation = m_stateEstimator->getEstimatedOrientation();

        double yaw = orientation.getYaw().valueRadians();

        math::Vector2 newVelocity_b(param, 0);
        math::Vector2 newVelocity_n = math::nRb(yaw) * newVelocity_b;

        if(newVelocity_b[0] <= m_maxSpeed && newVelocity_b[0] >= m_minSpeed)
        {
            m_controller->translate(position, newVelocity_n);
            m_velocity_b[0] = newVelocity_b[0];
        } else
        {
        }

        break;
    }

    case CMD_TSETSPEED:
    {
        // get the current estimated position
        math::Vector2 position = m_stateEstimator->getEstimatedPosition();
        
        // get the current desired yaw
        math::Quaternion orientation = m_stateEstimator->getEstimatedOrientation();

        double yaw = orientation.getYaw().valueRadians();

        math::Vector2 newVelocity_b(0, param);
        math::Vector2 newVelocity_n = math::nRb(yaw) * newVelocity_b;

        if(newVelocity_b[1] <= m_maxSpeed && newVelocity_b[1] >= m_maxSpeed)
        {
            m_controller->translate(position, newVelocity_n);
            m_velocity_b[1] = newVelocity_b[1];
        } else
        {
        }
        break;
    }
    case CMD_NOTHING:
    {
        // Ignore, just sent to keep the connection alive
        break;
    }

    case CMD_ANGLEYAW:
    {
        if(param != 0)
        {
            double yaw = param / m_yawGain;
            m_controller->yawVehicle(yaw, 0);
        }
        break;
    }

    case CMD_ANGLEPITCH:
    {
        if(param != 0)
        {
            double pitch = param / m_pitchGain;
//                printf("P: %f\n", pitch);
            m_controller->pitchVehicle(pitch, 0);
        }
        break;
    }

    case CMD_ANGLEROLL:
    {
        if(param != 0)
        {
            double roll = param / m_rollGain;
            m_controller->rollVehicle(roll, 0);
//                printf("R: %f\n", roll);
        }
        break;
    }


    default:
    {
        //printf("Invalid network command type: %c\n", cmd);
    }

    }
    
    // Return true to keep running
    return true;
}

} // namespace network
} // namespace ram

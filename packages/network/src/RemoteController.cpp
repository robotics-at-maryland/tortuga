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
    m_sockfd(-1),
    m_controller(core::Subsystem::getSubsystemOfType<control::IController>(deps)),
    m_stateEstimator(core::Subsystem::getSubsystemOfType<estimation::IStateEstimator>(deps))
{
    assert(m_controller.get() != 0 && "Did not get controller");
    assert(m_stateEstimator.get() != 0 && "Did not get estimator");
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

    setupNetworking(m_port);
}

RemoteController::~RemoteController()
{
    if (-1 != m_sockfd)
    {
        close(m_sockfd);
        m_sockfd = -1;
    }
    unbackground();
}

void RemoteController::enable()
{
    // ok to call this multiple times in a row
    background(-1);
}

void RemoteController::disable()
{
    // Don't wait for it to stop
    unbackground();
}

void RemoteController::update(double)
{
    // Wait for command packet
    signed char buf[2];
    int len = recvfrom(m_sockfd, buf, 2, 0, NULL, NULL);
    if(len != 2)
    {
        // If error from network drop out of receive loop
        printf("Error reading from network\n");
    }
    //printf("Got message\n");

//    if (backgrounded())
//    {
        // Break out commands and parameters
        unsigned char cmd = buf[0];
        signed char param = buf[1];
        //printf("Procssing");
        // Process Packet (If quit message drop out of loop, stop running)
        processMessage(cmd, param);
//    }
}

void RemoteController::background(int interval)
{
    if (-1 == m_sockfd)
        setupNetworking(m_port);        
    core::Updatable::background(interval);
}

void RemoteController::unbackground(bool join)
{
    if (-1 != m_sockfd)
    {
        // Send a UDP packet to break out of the loop
        struct hostent* he = gethostbyname("localhost");
        assert(he != NULL && "error with get host");
        
        int outSock = socket(AF_INET, SOCK_DGRAM, 0);
        assert(outSock >= 0 && "error creating socket");

        struct sockaddr_in their_addr; // connector's address information
        memset((void *)&their_addr, 0, sizeof(their_addr));// zero
        their_addr.sin_family = AF_INET;    // host byte order
        their_addr.sin_port = htons(m_port);  // short, network byte order
        their_addr.sin_addr = *((struct in_addr *)he->h_addr);
        
        signed char buf[2] = {0, 0};
        sendto(outSock, buf, 2, 0, (struct sockaddr *) &their_addr,
               sizeof(struct sockaddr_in));
        
        close(outSock);
    }
    
    core::Updatable::unbackground(join);
    
    if (-1 != m_sockfd)
    {   
        close(m_sockfd);
        m_sockfd = -1;
    }
}

void RemoteController::setupNetworking(boost::int16_t port)
{
    // Grab socket to listen on
    if ((m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        assert(false && "Socket creation failed");
    }

    // Setup my address information
    struct sockaddr_in addr;
    memset((void *)&addr, 0, sizeof(addr)); /* zero */
    addr.sin_family = AF_INET;    /* host byte order */
    addr.sin_port = htons(port);  /* short, network byte order */
    addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */

    // Bind the socket to the address
    int ret = bind(m_sockfd,(struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (ret < 0)
    {
        perror("bind");
        assert(false && "Binding to port failed");
    }

/*
  unsigned char buf[65536];
  struct sockaddr_in addr = {{0}, 0};
  socklen_t t = sizeof(addr);
  addr.sin_port = 0;
  addr.sin_addr.s_addr = INADDR_ANY;
  // Return length is ignored
  recvfrom(sockfd, buf, 65536, 0, (struct sockaddr *) &addr, &t);

  printf("Recieved first packet.\n");*/
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
        // get the current desired velocity (inertial frame)
        math::Vector2 velocity = m_controller->getDesiredVelocity();

        // get the current estimated position
        math::Vector2 position = m_stateEstimator->getEstimatedPosition();
        
        // get the current desired yaw
        double yaw = m_stateEstimator->getEstimatedOrientation().getYaw().valueRadians();

        // define a vector for the velocity increase (body frame)
        math::Vector2 vInc_b(m_speedEnc, 0);

        // rotate the incremental change to the inertial frame
        math::Vector2 vInc_n = math::nRb(yaw) * vInc_b;
        math::Vector2 newVelocity = velocity + vInc_n;

        if(newVelocity[0] < m_maxSpeed && 
           newVelocity[1] < m_maxSpeed)
            m_controller->translate(position, newVelocity);

        //printf("\nNEW SPEED:  %f\n", m_controller->getSpeed());
        break;
    }

    case CMD_DECSPEED:
    {

        // get the current desired velocity (inertial frame)
        math::Vector2 velocity = m_controller->getDesiredVelocity();

        // get the current estimated position
        math::Vector2 position = m_stateEstimator->getEstimatedPosition();
        
        // get the current desired yaw
        double yaw = m_stateEstimator->getEstimatedOrientation().getYaw().valueRadians();

        // define a vector for the velocity increase (body frame)
        math::Vector2 vInc_b(-m_speedEnc, 0);

        // rotate the incremental change to the inertial frame
        math::Vector2 vInc_n = math::nRb(yaw) * vInc_b;
        math::Vector2 newVelocity = velocity + vInc_n;

        if(newVelocity[0] > m_minSpeed && 
           newVelocity[1] > m_minSpeed)
            m_controller->translate(position, newVelocity);

        //printf("\nNEW SPEED:  %f\n", m_controller->getSpeed());
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

        if(newVelocity_n[0] <= m_maxSpeed && newVelocity_n[0] >= m_minSpeed &&
           newVelocity_n[1] <= m_maxSpeed && newVelocity_n[1] >= m_maxSpeed)
        {
            m_controller->translate(position, newVelocity_n);
            //printf("\nNEW SPEED:  %f\n", m_controller->getSpeed());
        } else
        {
            //printf("\nINVALID NEW SPEED: %d\n", param);
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

        if(newVelocity_n[0] <= m_maxSpeed && newVelocity_n[0] >= m_minSpeed &&
           newVelocity_n[1] <= m_maxSpeed && newVelocity_n[1] >= m_maxSpeed)
        {
            m_controller->translate(position, newVelocity_n);
            //printf("\nNEW SPEED:  %f\n", m_controller->getSpeed());
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
//                printf("Y: %f\n", yaw);
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

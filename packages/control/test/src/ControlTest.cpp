/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/ControlTest.cpp
 */

// STD Includes
#include <iostream>

// Networking includes
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

/* timing includes */
#include <time.h>
#include <sys/time.h>



// Library Includes
#include <boost/program_options.hpp>

// Project Includes
#include "core/include/Application.h"
#include "control/include/IController.h"


#define MYPORT 9219 /*YOUR PORT HERE*/
/* the port users will be connecting to */

#define BACKLOG 10  /* how many pending connections queue will hold */

#define DEADMAN_WAIT 1

// Setting for the hand control inputs
#define MAX_DEPTH 5

#define MIN_DEPTH 0

#define DEPTH_ENC 0.25
#define TURN_ENC 10

#define MIN_SPEED -5
#define MAX_SPEED 5
#define SPEED_ENC 1

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


using namespace std;
using namespace ram;
namespace po = boost::program_options;


/// Global File Descriptor for current comms socket
int g_teleopFD = 0;

/// Global status of the run loop
bool g_running = true;

/** This handles shutdown of the system by a signal
 *
 * This is ever called network comms have stopped for too long, or if the user
 * has pressed CTRL-C at the command line.
 * */
void shutdownHandler(int i);

/** Sets up the networking based on the setting defined above
 *
 * @return  A socket file descriptor to listen on, if < 0 there was an error.
 */
int setupNetworking(struct sockaddr_in& my_addr);

/** Enters an continous run loop which exits upon emergency shutdown, or CTRL-C
 *
 * @param controller  The conntroller we are commanding
 */
void networkLoop(control::IController* controller);

/** Process the incomming network messages
 *
 * @return If false it means an emergency shutdown command was sent
 */
bool processMessage(control::IController* controller, unsigned char cmd,
        signed char param=0);

int main(int argc, char** argv)
{
    std::string configPath;
    po::options_description desc("Allowed options");
    po::variables_map vm;

    try
    {
        desc.add_options()
        ("help", "produce help message")
        ("configpath", po::value<std::string>(&configPath)->default_value("config.yml"),
                "config file path")
                ;

        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    ram::core::Application app(configPath);

    {
        ram::core::SubsystemPtr controller = app.getSubsystem("Controller");

        // Got into networking loop
        networkLoop(dynamic_cast<ram::control::IController*>(controller.get()));
    }

    // Sleep To Allow thrusters to stop
    sleep(1);
};

void shutdownHandler(int i)
{
    if (SIGALRM == i)
        printf("\nKeep-alive timer expired!\n");
    else
        printf("\nCtrl-C Pressed, shutting down\n");
    g_running = false;
    close(g_teleopFD);

    // Turn off the alarm signal handler
    signal(SIGALRM, SIG_DFL);
}


int setupNetworking(struct sockaddr_in& my_addr)
{
    // listen on sock_fd
    int sockfd;
    unsigned char buf[65536];

    // Grab socket to listen on
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        return sockfd;
    }

    // Setup my address information
    memset((void *)&my_addr,0,sizeof(my_addr)); /*zero*/
    my_addr.sin_family = AF_INET;      /* host byte order */
    my_addr.sin_port = htons(MYPORT);  /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */

    // Bind the socket to the address
    int ret = bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr));
    if (ret < 0)
    {
        perror("bind");
        return ret;
    }

    size_t t = sizeof(my_addr);
    my_addr.sin_port = 0;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    // Return length is ignored
    recvfrom(sockfd, buf, 65536, 0, (struct sockaddr *) &my_addr, &t);

    printf("Recieved first packet.\n");

    return sockfd;
}

void networkLoop(control::IController* controller)
{
    struct sockaddr_in my_addr;
    int sockfd = setupNetworking(my_addr);
    if (sockfd < 0)
    {
        printf("Error setting up networking\n");
        return;
    }

    // Setup deadman timer and CTRL-C handler
    signal(SIGALRM, shutdownHandler);
    signal(SIGINT, shutdownHandler);

    // Recored the FD to allow external shutdown
    g_teleopFD = sockfd;
    printf("server: got connection\n");

    // Activate deadman timer
    alarm(DEADMAN_WAIT);

    // Got connection, jump into recive loop
    while(g_running)
    {

        // Wait for command packet
        signed char buf[2];

        size_t t = sizeof(my_addr);
        my_addr.sin_port = 0;
        my_addr.sin_addr.s_addr = INADDR_ANY;
        int len = recvfrom(sockfd, buf, 2, 0, (struct sockaddr *) &my_addr, &t);



        if(len != 2)
        {
            // If error from network drop out of receive loop
            printf("Error reading from network\n");
            break;
        }

        unsigned char cmd = buf[0];
        signed char param = buf[1];

        // Reset the deadman timer
        alarm(DEADMAN_WAIT);


        struct timeval tv;
        gettimeofday(&tv, NULL);

        int startTime=0, endTime=0;

        startTime = tv.tv_usec;

        // Process Packet (If quit message drop out of loop, stop running)
        g_running = processMessage(controller, cmd, param);

        gettimeofday(&tv, NULL);
        endTime = tv.tv_usec;

        if(startTime > endTime) /* Wraparound offset */
            endTime += 1048576;

	if(cmd != CMD_NOTHING)
	    printf("processMessage took %d usec\n", endTime - startTime);

    }

    // Close old fd
    close(sockfd);

    printf("server: lost connection\n");

}


bool processMessage(control::IController* controller, unsigned char cmd,
        signed char param)
{
    switch(cmd)
    {
        case CMD_EMERGSTOP:
        {
            printf("Emergency stop received\n");
            // Return false to stop the main network loop
            return false;
            break;
        }

        case CMD_TURNLEFT:
        {
            printf("Yaw left\n");
            controller->yawVehicle(TURN_ENC);
            break;
        }

        case CMD_TURNRIGHT:
        {
            printf("Yaw right\n");
            controller->yawVehicle(-TURN_ENC);
            break;
        }

        case CMD_INCSPEED:
        {
            if(controller->getSpeed() < MAX_SPEED)
                controller->setSpeed(controller->getSpeed()+SPEED_ENC);

            printf("\nNEW SPEED:  %d\n", controller->getSpeed());
            break;
        }

        case CMD_DECSPEED:
        {
            if(controller->getSpeed() > MIN_SPEED)
                controller->setSpeed(controller->getSpeed()-SPEED_ENC);

            printf("\nNEW SPEED:  %d\n", controller->getSpeed());
            break;
        }

        case CMD_DESCEND:
        {
            if(controller->getDepth() < MAX_DEPTH)
                controller->setDepth(controller->getDepth()+DEPTH_ENC);

            printf("NEW DEPTH: %f\n", controller->getDepth());
            break;
        }

        case CMD_ASCEND:
        {
            if(controller->getDepth() > MIN_DEPTH)
                controller->setDepth(controller->getDepth()-DEPTH_ENC);


            printf("NEW DEPTH: %f\n", controller->getDepth());
            break;
        }

        case CMD_ZEROSPEED:
        {
            controller->setSpeed(0);
            printf("\nNEW SPEED:  %d\n", controller->getSpeed());
            break;
        }

        case CMD_SETSPEED:
        {
            if(param <= MAX_SPEED && param >= MIN_SPEED)
            {
                controller->setSpeed(param);
                printf("\nNEW SPEED:  %d\n", controller->getSpeed());
            } else
            {
                printf("\nINVALID NEW SPEED: %d\n", param);
            }
        }

        case CMD_NOTHING:
        {
            // Ignore, just sent to keep the connection alive
            break;
        }

        default:
        {
            printf("Invalide network command type: %c\n", cmd);
        }
    }

    // Return true to keep running
    return true;
}

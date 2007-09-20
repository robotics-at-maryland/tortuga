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

// Library Includes
#include <boost/program_options.hpp>

// Project Includes
#include "control/include/BWPDController.h"

#include "core/include/ConfigNode.h"

#include "vehicle/include/Vehicle.h"
#include "vehicle/include/device/Thruster.h"
#include "vehicle/include/device/IMU.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MYPORT 9219 /*YOUR PORT HERE*/
                /* the port users will be connecting to */

#define BACKLOG 10  /* how many pending connections queue will hold */





using namespace std;
using namespace ram;
namespace po = boost::program_options;

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

    // Parse Some config stuff
    core::ConfigNode root = core::ConfigNode::fromFile(configPath);
    core::ConfigNode modules(root["Modules"]);

    // Create vehicle and devices
    core::ConfigNode veh_cfg(modules["Vehicle"]);
    vehicle::Vehicle vehicle(veh_cfg);
    core::ConfigNode dev_cfg(veh_cfg["Devices"]);

    // Set all the device names
    dev_cfg["StarboardThruster"].set("name", "StarboardThruster");
    dev_cfg["ForeThruster"].set("name", "ForeThruster");
    dev_cfg["AftThruster"].set("name", "AftThruster");
    dev_cfg["PortThruster"].set("name", "PortThruster");
    dev_cfg["IMU"].set("name", "IMU");

//    vehicle::device::ThrusterPtr thruster =
//        vehicle::device::Thruster::construct(dev_cfg["StarboardThruster"]);

//    cout << "Name: " << thruster->getName() << endl;
//    vehicle._addDevice(thruster);
//    cout << "Test " << vehicle.getDevice("StarboardThruster")->getName() << endl;
    vehicle._addDevice(vehicle::device::Thruster::construct(
                           dev_cfg["StarboardThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(
                                          dev_cfg["ForeThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(
                                          dev_cfg["AftThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(
                                          dev_cfg["PortThruster"]));
    vehicle._addDevice(vehicle::device::IMU::construct(dev_cfg["IMU"]));

    int thrustUpdate = dev_cfg["StarboardThruster"]["update_interval"].asInt();
    int vehicleUpdate = dev_cfg["IMU"]["update_interval"].asInt();
    int imuUpdate = dev_cfg["IMU"]["update_interval"].asInt();

    // Create our controller
    core::ConfigNode ctrl_cfg(modules["Controller"]);
    control::BWPDController controller(&vehicle, ctrl_cfg);
    controller.background(ctrl_cfg["update_interval"].asInt());

    // Start up the devices in the background
    vehicle.getDevice("StarboardThruster")->background(thrustUpdate);
    vehicle.getDevice("IMU")->background(imuUpdate);
    vehicle.background(vehicleUpdate);



    int sockfd, new_fd;/*listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /*my address information */
    struct sockaddr_in their_addr; /*connector's address information */
    int sin_size;
    int ret;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    memset((void *)&my_addr,0,sizeof(my_addr)); /*zero*/
    my_addr.sin_family = AF_INET;      /* host byte order */
    my_addr.sin_port = htons(MYPORT);  /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */

    ret = bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr));
    if (ret < 0)
    {
        perror("bind");
        return ret;
    }
    ret = listen(sockfd, BACKLOG);
    if (ret < 0)
    {
         perror("listen");
         return ret;
    }


    while(1)
    {  /* main accept() loop */
        sin_size = sizeof(struct sockaddr_in);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, (socklen_t *) &sin_size);
        if (new_fd < 0) {
               perror("accept");
               continue;
        }

        printf("server: got connection\n");
        {
		int fd = new_fd;
		//unsigned char buf[64];
		unsigned char type = 0;
	    	unsigned char num = 0;
	        signed short val = 0;

		while(1)
		{
			if(recv(fd, &type, 1, 0) != 1)
				exit(1);

			recv(fd, &num, 1, 0);
			recv(fd, &val, 2, 0);   /* We're both Intel, so who cares about byte order */

//			printf("%d %d: %d\n", type, num, val);
#define BTN_UP 2
#define BTN_DOWN 3
#define BTN_LEFT 8
#define BTN_RIGHT 6

#define BTN_SHALLOW 9
#define BTN_DEEP 5

#define MAX_DEPTH 5

#define MIN_DEPTH 0

#define DEPTH_ENC 0.25
#define TURN_ENC 10
#define MIN_SPEED -5
#define MAX_SPEED 5


//#warning THE FOLLOWING BLOCK OF CODE HAS NOT YET BEEN TESTED ON THE VEHICLE
            if(type == 0 && num == 1) // First stick, vertical axis
            {
                int newSpeed = val / (32768 / MAX_SPEED);

                if(newSpeed != controller.getSpeed())
                {
                    printf("NEW SPEED:  %d\n", newSpeed);
                    controller.setSpeed(newSpeed);
                }
            }




			if(type == 1 && val == 1) // A button press
			{
				switch(num)	/* Which button? */
				{
					case 11:
					{
						return 1;
						break;
					}
/*
					case BTN_UP:
					{
						if(controller.getSpeed() < MAX_SPEED)
							controller.setSpeed(controller.getSpeed()+SPEED_ENC);

						printf("\nNEW SPEED:  %d\n", controller.getSpeed());
						break;
					}

					case BTN_DOWN:
					{
                                                if(controller.getSpeed() > MIN_SPEED)
							controller.setSpeed(controller.getSpeed()-SPEED_ENC);

						printf("\nNEW SPEED:  %d\n", controller.getSpeed());
						break;
					}
*/
					case BTN_LEFT:
					{
						controller.yawVehicle(TURN_ENC);
						break;
					}


                    case BTN_RIGHT:
                    {
	                    controller.yawVehicle(-TURN_ENC);
	                    break;
	                }


                    case BTN_DEEP:
				    {
					    if(controller.getDepth() < MAX_DEPTH)
						    controller.setDepth(controller.getDepth()+DEPTH_ENC);

						printf("NEW DEPTH: %f\n", controller.getDepth());
						break;
					}

					case BTN_SHALLOW:
                    {
				        if(controller.getDepth() > MIN_DEPTH)
				            controller.setDepth(controller.getDepth()-DEPTH_ENC);


						printf("NEW DEPTH: %f\n", controller.getDepth());
					    break;
				    }

				}
			}
		}

		exit(0);
        }
        close(new_fd);  /* parent doesn't need this */
        while(waitpid(-1,NULL,WNOHANG) > 0); /*clean up child proc*/
    }



    // Sleep for fifteen minutes
    sleep(15 * 60);
};

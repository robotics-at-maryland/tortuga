/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/test/src/ThrusterDemo.cpp
 */

// UNIX Includes
#include <unistd.h>

// STD Includes
#include <iostream>
#include <vector>

// Library Includes
#include <boost/assign/list_of.hpp> // for 'list_of()'

// Project Includes
#include "vehicle/include/device/Thruster.h"
#include "core/include/ConfigNode.h"

using namespace std;
namespace ba = boost::assign;
using namespace ram;
using namespace ram::core;
using namespace ram::vehicle::device;

static const int FORCE = 6;

static const std::string PORT_CONFIG("{'name' : 'PortThruster',"
                                     " 'address' : 3,"
                                     " 'calibration_factor': 0.3652,"
                                     " 'direction' : -1}");
static const std::string STAR_CONFIG("{'name' : 'StarboardThruster',"
                                     " 'address' : 4,"
                                     " 'calibration_factor': 0.4146,"
                                     " 'direction' : -1}");
static const std::string FORE_CONFIG("{'name' : 'ForeThruster',"
                                     " 'address' : 2,"
                                     " 'calibration_factor': 0.4368,"
                                     " 'direction' : -1}");
static const std::string AFT_CONFIG("{'name' : 'AftThruster',"
                                    " 'address' : 1,"
                                    " 'calibration_factor': 0.3612,"
                                    " 'direction' : 1}");
//                                       calibration_factor
int main()
{
    // Create each thruster and add them to a list
    cout << "Creating all four thrusters" << endl;
    Thruster port(ConfigNode::fromString(PORT_CONFIG));
    Thruster starboard(ConfigNode::fromString(STAR_CONFIG));
    Thruster fore(ConfigNode::fromString(FORE_CONFIG));
    Thruster aft(ConfigNode::fromString(AFT_CONFIG));

    std::vector<Thruster*> list = ba::list_of(&port)(&starboard)(&fore)(&aft);

    // Start up the background thread to send off commands to the Motor Ctrl.
    // currently it wakes up every 100 ms, it currently just process messages
    // then goes back to waiting, but in the future periodic tasks will be
    // done there
    port.background(100);
    
    // Loop through each thruster and send it two commands.
    for (size_t i = 0; i < list.size(); ++i)
    {
        cout << "Sending: \"" << list[i]->getName() << " " << FORCE
             << " newtons" << endl;
        list[i]->setForce(FORCE);
        usleep(5000 * 1000);
        cout << "Stopping" << endl;
        list[i]->setForce(0);
    }

    for (size_t i = 0; i < list.size(); ++i)
        list[i]->setForce(FORCE);
    
    
    // When the thrusters all are desctruted they shutdown the communicator,
    // which in turns shuts down the background threadxb
    
    return 0;
}

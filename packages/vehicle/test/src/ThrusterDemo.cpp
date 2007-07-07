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

static const std::string PORT_CONFIG("{'name' : 'PortThruster',"
                                     " 'address' : '01',"
                                     " 'calibration_factor': 0.3652 }");
static const std::string STAR_CONFIG("{'name' : 'StarboardThruster',"
                                     " 'address' : '02',"
                                     " 'calibration_factor': 0.4146 }");
static const std::string FORE_CONFIG("{'name' : 'ForeThruster',"
                                     " 'address' : '03',"
                                     " 'calibration_factor': 0.4368 }");
static const std::string AFT_CONFIG("{'name' : 'AftThruster',"
                                    " 'address' : '04',"
                                    " 'calibration_factor': 0.3612 }");
//                                       calibration_factor
int main()
{
    // Create each thruster and add them to a list
    cout << "Creating all four thrusters" << endl;
    Thruster port(0, ConfigNode::fromString(PORT_CONFIG));
    Thruster starboard(0, ConfigNode::fromString(STAR_CONFIG));
    Thruster fore(0, ConfigNode::fromString(FORE_CONFIG));
    Thruster aft(0, ConfigNode::fromString(AFT_CONFIG));

    std::vector<Thruster*> list = ba::list_of(&port)(&starboard)(&fore)(&aft);

    // Start up the background thread to send off commands to the Motor Ctrl.
    // currently it wakes up every 100 ms, it currently just process messages
    // then goes back to waiting, but in the future periodic tasks will be
    // done there
    port.background(100);
    
    // Loop through each thruster and send it two commands.
    for (size_t i = 0; i < list.size(); ++i)
    {
        cout << "Sending: \"" << list[i]->getName() << " 1 newton" << endl;
        list[i]->setForce(250);
        usleep(2000 * 1000);

        cout << "Stopping" << endl;
        list[i]->setForce(0);
    }

    // When the thrusters all are desctruted they shutdown the communicator,
    // which in turns shuts down the background threadxb
    
    return 0;
}

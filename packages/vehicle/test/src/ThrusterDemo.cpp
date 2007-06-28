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

using namespace std;
namespace ba = boost::assign;
using namespace ram::vehicle::device;

int main()
{
    // Create each thruster and add them to a list
    cout << "Creating all four thrusters" << endl;
    Thruster port("PortThruster", "01", 0.9);
    Thruster starboard("StarboardThruster", "02", 0.9);
    Thruster fore("ForeThruster", "03", 0.9);
    Thruster aft("AftThruster", "04", 0.9);

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

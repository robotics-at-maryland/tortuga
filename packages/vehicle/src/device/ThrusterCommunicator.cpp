/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/ThrusterCommunicator.cpp
 */

// STD Includes
#include <sstream>
#include <iostream>
#include <cstdio>

// UNIX Includes (needed for serial stuff)
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <poll.h>

// Library Includes
#include <boost/algorithm/string.hpp>

// Project Includes
#include "vehicle/include/device/ThrusterCommunicator.h"
#include "vehicle/include/device/ThrusterCommand.h"
#include "vehicle/include/device/Thruster.h"
#include "thrusterapi/include/thrusterapi.h"

namespace ram {
namespace vehicle {
namespace device {

static const int MOTOR_CMD_TIMEOUT = 1000;
static const char* MOTOR_CONTROLLER_DEV_FILE = "/dev/motor";
    
template<> ThrusterCommunicator* 
    pattern::Singleton<ThrusterCommunicator>::ms_Singleton = 0;
    
ThrusterCommunicator& ThrusterCommunicator::getSingleton()
{  
    assert(ms_Singleton && 
           "Singleton instance of ThrusterCommunicator not created");  
    return *ms_Singleton;    
}

ThrusterCommunicator* ThrusterCommunicator::getSingletonPtr()
{
    return ms_Singleton;
}  

ThrusterCommunicator::ThrusterCommunicator() :
    m_serialFD(-1)
{
    // Open and store my serial port FD here
    m_serialFD = openThrusters(MOTOR_CONTROLLER_DEV_FILE);

    if (m_serialFD < 0)
        std::cout << "Could not open serial port" << std::endl;
    
    std::cout << "Thruster Communicator Created" << std::endl;
}

ThrusterCommunicator::~ThrusterCommunicator()
{
    close(m_serialFD);
    
    // Process all pending messages, then shutdown message thread
    processCommands();
    
    unbackground(true);
    // close my serial port FD here
    std::cout << "Thruster Communicator Shutdown" << std::endl;
}
    
void ThrusterCommunicator::registerThruster(Thruster* thruster)
{
    if (0 == ThrusterCommunicator::getSingletonPtr())
    {
        // Create communicator singleton (Move me somewhere else?)
        new ThrusterCommunicator();
    }
    
    ThrusterCommunicator::getSingleton().addThruster(thruster);
}


void ThrusterCommunicator::unRegisterThruster(Thruster* thruster)
{
    ThrusterCommunicator::getSingleton().removeThruster(thruster);
}

void ThrusterCommunicator::sendThrusterCommand(ThrusterCommandPtr cmd)
{
//    std::cout << "Thrusting" << std::endl;
    m_commandQueue.push(cmd);
}

void ThrusterCommunicator::update(double timestep)
{
    // Add any timed task here

    // Process all pending commands (there really shouldn't be any)     
    processCommands();
}
    
void ThrusterCommunicator::waitForUpdate(long microseconds)
{
    // This time format is UNIX dependent
    boost::xtime xtime;
    xtime.sec = 0;
    xtime.nsec = microseconds * 1000;

    // Wait for a new message
    ThrusterCommandPtr newCommand;
    if(m_commandQueue.popTimedWait(xtime, newCommand))
    {
        // Run the current command then process the rest
        runCommand(newCommand);
        processCommands();
    }
}
    
void ThrusterCommunicator::addThruster(Thruster* thruster)
{
    m_thrusters.insert(m_thrusters.begin(), thruster);
}

void ThrusterCommunicator::removeThruster(Thruster* thruster)
{
    ThrusterSetIter iter = m_thrusters.find(thruster);

    assert(iter != m_thrusters.end() && "Thruster not registered");
    m_thrusters.erase(iter);

    // If there are no more thrusters left, just delete the communicator
    if (0 == m_thrusters.size())
        delete this;
}

void ThrusterCommunicator::processCommands()
{
    ThrusterCommandPtr nextCommand;

    // Only process commands if the port is actually open
    if (m_serialFD >= 0)
    {
        // Runs every command untill the queue is empty
        while(m_commandQueue.popNoWait(nextCommand))
        {
            runCommand(nextCommand);
        }
    }
    // Port not open, attempt to open it up and reprocess commands, or just
    // drop them
    else
    {
        m_serialFD = openThrusters(MOTOR_CONTROLLER_DEV_FILE);

        // Good connection, run commands
        if (m_serialFD >= 0)
        {
            processCommands();
        }
        // Bad connection, drop all commands
        else
        {
            while(m_commandQueue.popNoWait(nextCommand)) {}
        }
    }
}
    
void ThrusterCommunicator::runCommand(ThrusterCommandPtr command)
{
    if (m_serialFD >= 0)
    {
        int ret = command->run(m_serialFD);        
	if (ret != 0)
            std::cout << "Write failure" << std::endl;
    }
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

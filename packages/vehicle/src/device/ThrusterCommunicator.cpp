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

// Project Includes
#include "vehicle/include/device/ThrusterCommunicator.h"
#include "vehicle/include/device/ThrusterCommand.h"

namespace ram {
namespace vehicle {
namespace device{

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

void ThrusterCommunicator::sendThrusterCommand(ThrusterCommand* cmd)
{
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
    ThrusterCommand* newCommand;
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
    std::set<Thruster*>::iterator iter = m_thrusters.find(thruster);

    assert(iter != m_thrusters.end() && "Thruster not registered");
    m_thrusters.erase(iter);

    // If there are no more thrusters left, just delete the communicator
    if (0 == m_thrusters.size())
        delete this;
}

void ThrusterCommunicator::processCommands()
{
    ThrusterCommand* nextCommand;

    // Runs every command untill the queue is empty
    while(m_commandQueue.popNoWait(nextCommand))
    {
        runCommand(nextCommand);
    }
}
    
void ThrusterCommunicator::runCommand(ThrusterCommand* command)
{
    std::stringstream ss;

    ss << command->getCommandType() << command->getAddress()
       << command->getArgs() << "\n\r";
    
    /// TODO: Send string over serial
    std::cout << "Thruster command: " << ss.str() << std::endl;
    
    delete command;
}
    
} // namespace device
} // namespace vehicle
} // namespace ram

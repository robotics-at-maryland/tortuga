/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/ThrusterCommand.cpp
 */

// Project Includes
#include "vehicle/include/device/ThrusterCommand.h"

namespace ram {
namespace vehicle {
namespace device{

ThrusterCommand::ThrusterCommand(std::string address, std::string commandType,
                                 std::string args, int sleepTime) :
    m_address(address),
    m_commandType(commandType),
    m_args(args),
    m_sleepTime(sleepTime)
{
}

std::string ThrusterCommand::getAddress()
{
    return m_address;
}

std::string ThrusterCommand::getCommandType()
{
    return m_commandType;
}
    
std::string ThrusterCommand::getArgs()
{
    return m_args;
}


int ThrusterCommand::getSleepTime()
{
    return m_sleepTime;
}

} // namespace device
} // namespace vehicle
} // namespace ram

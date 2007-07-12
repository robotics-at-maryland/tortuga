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
#include "thrusterapi/include/thrusterapi.h"

namespace ram {
namespace vehicle {
namespace device{

ThrusterCommand::ThrusterCommand(int address, int commandType,
                                 boost::any args) :
    m_address(address),
    m_commandType(commandType),
    m_args(args)
{
}

ThrusterCommandPtr ThrusterCommand::construct(int address, int commandType,
                                              boost::any args)
{
    return ThrusterCommandPtr(new ThrusterCommand(address, commandType, args));
}
    
int ThrusterCommand::run(int thrusterFD)
{
    switch (m_commandType)
    {
        case ThrusterCommand::RESET:
        {
            // Fill me in
        }
        break;
            
        case ThrusterCommand::SPEED:
        {
            return setSpeed(thrusterFD, m_address,
                            boost::any_cast<int>(m_args));
        }
        break;
    }
    
    return -1;
}

} // namespace device
} // namespace vehicle
} // namespace ram

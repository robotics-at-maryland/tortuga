/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/include/device/ThrusterCommand.h
 */

#ifndef RAM_CORE_THRUSTERCOMMAND_06_24_2007
#define RAM_CORE_THRUSTERCOMMAND_06_24_2007

// STD Includes
#include <string>

// Library Includes
#include <boost/any.hpp>

// Project Includes
#include "vehicle/include/device/Common.h"

namespace ram {
namespace vehicle {
namespace device {

class ThrusterCommand
{
public:
    enum COMMAND_TYPE {
        RESET,
        SPEED
    };
    
    /** Creates a command to be run on the thruster
     */
    ThrusterCommand(int address, int command_type, boost::any args);

    static ThrusterCommandPtr construct(int address, int command_type,
                                        boost::any args = 0);
    
    int run(int thrusterFD);
    
private:
    int m_address;
    int m_commandType;
    boost::any m_args;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram
    
#endif // RAM_CORE_THRUSTERCOMMAND_06_24_2007

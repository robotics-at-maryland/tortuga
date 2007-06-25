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

namespace ram {
namespace vehicle {
namespace device{

class ThrusterCommand
{
public:
    /** Creates a command to be run on the thruster
     */
    ThrusterCommand(std::string address, std::string commandType,
                    std::string args);

    std::string getAddress();
    std::string getCommandType();
    std::string getArgs();
    
private:
    std::string m_address;
    std::string m_commandType;
    std::string m_args;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram
    
#endif // RAM_CORE_THRUSTERCOMMAND_06_24_2007

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
#include <boost/program_options.hpp>
#include <boost/assign/list_of.hpp> // for 'list_of()'

// Project Includes
#include "math/include/Vector3.h"
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/device/Thruster.h"
#include "core/include/ConfigNode.h"

using namespace std;
namespace ba = boost::assign;
namespace po = boost::program_options;
using namespace ram;
using namespace ram::core;
using namespace ram::vehicle::device;

static const int FORCE = 6;
static const int APPLY_SLEEP = 10;
static const int POST_CMD_SLEEP = 1;

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

    // Apply a forward force
    std::cout << "Applying forward force" << std::endl;
    vehicle.applyForcesAndTorques(math::Vector3(FORCE, 0, 0),
                                  math::Vector3::ZERO);
    vehicle.update(0);
    sleep(APPLY_SLEEP);
    std::cout << "Stoping thrusters" << std::endl;
    vehicle.applyForcesAndTorques(math::Vector3::ZERO, math::Vector3::ZERO);
    vehicle.update(0);
    sleep(POST_CMD_SLEEP);
    std::cout << "Thrusters stopped" << std::endl;
    
    // Apply a forward force
    std::cout << "Applying upward force" << std::endl;
    vehicle.applyForcesAndTorques(math::Vector3(0, 0, FORCE),
                                  math::Vector3::ZERO);
    vehicle.update(0);
    sleep(APPLY_SLEEP);
    std::cout << "Stoping thrusters" << std::endl;
    vehicle.applyForcesAndTorques(math::Vector3::ZERO, math::Vector3::ZERO);
    vehicle.update(0);
    sleep(POST_CMD_SLEEP);
    std::cout << "Thrusters stopped" << std::endl;
    
    return 0;
}

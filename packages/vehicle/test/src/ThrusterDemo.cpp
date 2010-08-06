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
#include "core/include/ConfigNode.h"

using namespace std;
namespace ba = boost::assign;
namespace po = boost::program_options;
using namespace ram;
using namespace ram::core;
using namespace ram::vehicle::device;

static const int FORCE = 10;
static const int APPLY_SLEEP = 7;
static const int POST_CMD_SLEEP = 1;

/** Quits if the sleep ends early */
void quitSleep(vehicle::Vehicle* vehicle, unsigned int seconds);

/** Applies the force to the vehicle, and then does a APPLY_SLEEP */
void testForce(vehicle::Vehicle* vehicle, math::Vector3 force,
               math::Vector3 torque);

/** Stops thrusters and then does a POST_CMD_SLEEP */
void stopThrusters(vehicle::Vehicle* vehicle);

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
    core::ConfigNode modules(root["Subsystems"]);

    // Create vehicle and devices
    modules["Vehicle"].set("name", "Vehicle");
    core::ConfigNode veh_cfg(modules["Vehicle"]);
    vehicle::Vehicle vehicle(veh_cfg);
    math::Vector3 force;
    vehicle.background(25);

    // Apply a forward force
    force = math::Vector3(FORCE, 0, 0);
    std::cout << "Applying forward force (move's vehicle forward): " << force 
	      << std::endl;
    std::cout << "Air/Water goes back" << std::endl;

    testForce(&vehicle, force, math::Vector3::ZERO);
    stopThrusters(&vehicle);
    
    // Apply a upward force
    force = math::Vector3(0, 0, FORCE);
    std::cout << "Applying upward force (moves vehicle up): " << force 
	      << std::endl;
    std::cout << "Air/Water goes down" << std::endl;

    testForce(&vehicle, force, math::Vector3::ZERO);
    stopThrusters(&vehicle);
    
    // Apply a positive pitch
    math::Vector3 torque = math::Vector3(0, FORCE, 0);
    std::cout << "Applying a positive pitch (vehicle nose down): " << torque 
	      << std::endl;
    std::cout << "Air/Water above fore, Air/Water below aft" << std::endl;

    testForce(&vehicle, math::Vector3::ZERO, torque);
    stopThrusters(&vehicle);

    // Apply a sideways force
    force = math::Vector3(0, FORCE, 0);
    std::cout << "Applying left force (moves vehicle left): "
              << force  << std::endl;
    std::cout << "Air/Water goes right" << std::endl;

    testForce(&vehicle, force, math::Vector3::ZERO);
    stopThrusters(&vehicle);
    
    // Apply a positive roll
    torque = math::Vector3(FORCE, 0, 0);
    std::cout << "Applying a positive roll (vehicle roll right): " << torque 
	      << std::endl;
    std::cout << "Air/Water left top, Air/Water right bottom" << std::endl;

    testForce(&vehicle, math::Vector3::ZERO, torque);
    stopThrusters(&vehicle);
    
    return EXIT_SUCCESS;
}

void quitSleep(vehicle::Vehicle* vehicle, unsigned int seconds)
{
    // Guard against re-entrance
    static bool failedSleep = false;
    
    unsigned int sleepTime = sleep(seconds);
    if (0 != sleepTime)
    {
        if (failedSleep)
        {
            std::cout << "Force quit, no wait for thruster stop" << std::endl;
            exit(EXIT_FAILURE);
        }
        
        std::cout << "Sleep stopped early, quiting" << std::endl;

        failedSleep = true;
        stopThrusters(vehicle);
        failedSleep = false;
        
        exit(EXIT_SUCCESS);
    }
}

void testForce(vehicle::Vehicle* vehicle, math::Vector3 force,
               math::Vector3 torque)
{
    vehicle->applyForcesAndTorques(force, torque);
    //vehicle->update(0);
    quitSleep(vehicle, APPLY_SLEEP);
}

void stopThrusters(vehicle::Vehicle* vehicle)
{
    std::cout << "Stoping thrusters" << std::endl;
    vehicle->applyForcesAndTorques(math::Vector3::ZERO, math::Vector3::ZERO);
    //vehicle->update(0);
    quitSleep(vehicle, POST_CMD_SLEEP);
    std::cout << "Thrusters stopped" << std::endl;
}


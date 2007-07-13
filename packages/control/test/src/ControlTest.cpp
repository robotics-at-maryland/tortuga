/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/ControlTest.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <boost/program_options.hpp>

// Project Includes
#include "control/include/BWPDController.h"

#include "core/include/ConfigNode.h"

#include "vehicle/include/Vehicle.h"
#include "vehicle/include/device/Thruster.h"
#include "vehicle/include/device/IMU.h"

using namespace ram;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    std::string configPath;
    
    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("configpath", po::value<std::string>(&configPath)->default_value("config.yml"),
             "config file path")
        ;

        po::variables_map vm;        
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }

    // Parse Some config stuff
    core::ConfigNode root = core::ConfigNode::fromFile(configPath);
    core::ConfigNode modules(root["Module"]);

    // Create vehicle and devices
    core::ConfigNode veh_cfg(modules["Vehicle"]);
    vehicle::Vehicle vehicle(veh_cfg);
    vehicle._addDevice(vehicle::device::Thruster::construct(&vehicle,
                                          veh_cfg["StarboardThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(&vehicle,
                                          veh_cfg["PortThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(&vehicle,
                                          veh_cfg["AftThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(&vehicle,
                                          veh_cfg["PortThruster"]));
    vehicle._addDevice(vehicle::device::IMU::construct(&vehicle,
                                                      veh_cfg["IMU"]));

    // Start up the devices in the background
    vehicle.getDevice("StarboardThruster")->background(
        veh_cfg["StarboardThruster"]["update_interval"].asInt());
    vehicle.getDevice("IMU")->background(
        veh_cfg["IMU"]["update_interval"].asInt());
    vehicle.background(
        veh_cfg["IMU"]["update_interval"].asInt());
    
    // Create our controller
    core::ConfigNode ctrl_cfg(modules["Controller"]);
    control::BWPDController controller(&vehicle, ctrl_cfg);
    controller.background(ctrl_cfg["update_interval"].asInt());

    // Sleep for fifteen minutes
    sleep(15 * 60);
};

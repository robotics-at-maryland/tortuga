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
    core::ConfigNode dev_cfg(veh_cfg["Devices"]);
    vehicle._addDevice(vehicle::device::Thruster::construct(
                                          dev_cfg["StarboardThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(
                                          dev_cfg["PortThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(
                                          dev_cfg["AftThruster"]));
    vehicle._addDevice(vehicle::device::Thruster::construct(
                                          dev_cfg["PortThruster"]));
    vehicle._addDevice(vehicle::device::IMU::construct(dev_cfg["IMU"]));

//    int thrustUpdate = dev_cfg["StarboardThruster"]["update_interval"].asInt();
//    int vehicleUpdate = dev_cfg["IMU"]["update_interval"].asInt();
//    int imuUpdate = dev_cfg["IMU"]["update_interval"].asInt();
        
    // Create our controller
    core::ConfigNode ctrl_cfg(modules["Controller"]);
    control::BWPDController controller(&vehicle, ctrl_cfg);
    controller.background(ctrl_cfg["update_interval"].asInt());

    // Start up the devices in the background
    //vehicle.getDevice("StarboardThruster")->background(thrustUpdate);
    //vehicle.getDevice("IMU")->background(imuUpdate);
    //vehicle.background(vehicleUpdate);

    // Sleep for fifteen minutes
    sleep(15 * 60);
};

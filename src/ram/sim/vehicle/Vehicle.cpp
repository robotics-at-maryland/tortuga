/*
 *  Vehicle.cpp
 *  ice
 *
 *  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/24/08.
 *  Copyright 2008 TRX Systems. All rights reserved.
 *
 */

#include <ram/sim/vehicle/Vehicle.h>
#include <iostream>

using namespace ram::sim::vehicle;

Vehicle::Vehicle(const std::string& mName) : name(mName)
{
  std::cout << "Created new vehicle with name: " << name << std::endl;
}

Vehicle::~Vehicle()
{
  std::cout << "Destroyed vehicle with name: " << name << std::endl;
}

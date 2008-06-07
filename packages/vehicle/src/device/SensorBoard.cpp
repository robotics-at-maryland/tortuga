/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SensorBoard.cpp
 */

// Project Includes
#include "vehicle/include/device/SensorBoard.h"

namespace ram {
namespace vehicle {
namespace device {

SensorBoard::SensorBoard(int deviceFD,
                         core::EventHubPtr eventHub = core::EventHubPtr())
{
    
}
    

SensorBoard::SensorBoard(core::ConfigNode config,
                         core::EventHubPtr eventHub = core::EventHubPtr(),
                         IVehiclePtr vehicle = IVehiclePtr())
{
    
}
    
SensorBoard::~SensorBoard()
{
    
}

void SensorBoard::update(double timestep)
{
    
}

double SensorBoard::getDepth()
{

}

void SensorBoard::setThrusterValue(int address, int count)
{

}

bool SensorBoard::isThrusterEnabled(int address)
{

}

void SensorBoard::setThrusterEnabled(bool state)
{

}
    
}
}
}

/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Eliot Rudnick-Cohen <erudnick@umd.edu>
 * All rights reserved.
 *
 * Author: Eliot Rudnick-Cohen <erudnick@umd.edu>
 * File:  packages/estimation/include/modules/RangeEstimation.cpp
 */

// STD Includes
#include <iostream>
// Library Includes
#include <log4cpp/Category.hh>

//Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/modules/RangeEstimationModule.h"
#include "vehicle/include/device/DVL.h"


static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstRange"));

namespace ram {
namespace estimation {

RangeModule::RangeModule(core::ConfigNode config,core::EventHubPtr eventHub,
                         EstimatedStatePtr estimatedState) :
    EstimationModule(eventHub, "RangeModule", estimatedState,
                     vehicle::device::IVelocitySensor::RAW_RANGE_UPDATE)
{
    LOGGER.info("% Name range1 range2 range3 range4 average");
}

    
void RangeModule::update(core::EventPtr event)
{
    vehicle::RawBottomRangeEventPtr rangeEvent = 
        boost::dynamic_pointer_cast<vehicle::RawBottomRangeEvent>(event);
    if(!rangeEvent)
    {
        LOGGER.warn("Invalid Event Type");
        return;
    }
    //currently this only does an average
    double avg = rangeEvent->rangeBeam1 + rangeEvent->rangeBeam2 
        + rangeEvent->rangeBeam3 + rangeEvent->rangeBeam4;
    avg = avg / 4;
       
    LOGGER.infoStream() << m_name << " " << rangeEvent->rangeBeam1 <<
        " " << rangeEvent->rangeBeam2 << " " << rangeEvent->rangeBeam3 << " " << 
        rangeEvent->rangeBeam4 << " "<< avg << " ";
            
    m_estimatedState->setEstimatedBottomRange(avg);
        

}




}

}

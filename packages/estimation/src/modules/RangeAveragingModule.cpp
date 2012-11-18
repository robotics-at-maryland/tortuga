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
#include "estimation/include/modules/RangeAveragingModule.h"
#include "vehicle/include/device/DVL.h"


static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstAvgRange"));

namespace ram {
namespace estimation {

RangeAveragingModule::RangeAveragingModule(core::ConfigNode config,
                                           core::EventHubPtr eventHub,
                         EstimatedStatePtr estimatedState) :
    EstimationModule(eventHub, "RangeModule", estimatedState,
                     vehicle::device::IVelocitySensor::RAW_RANGE_UPDATE)
{
    LOGGER.info("% Name range1 range2 range3 range4 average");
}

    
void RangeAveragingModule::update(core::EventPtr event)
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
    filter.addValue(avg);

    LOGGER.infoStream() << m_name << " " << rangeEvent->rangeBeam1 <<
        " " << rangeEvent->rangeBeam2 << " " << rangeEvent->rangeBeam3 << " " << 
        rangeEvent->rangeBeam4 << " "<< filter.getValue() << " ";
            
    m_estimatedState->setEstimatedBottomRange(filter.getValue());
        

}



}
}

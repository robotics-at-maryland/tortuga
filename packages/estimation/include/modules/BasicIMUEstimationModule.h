/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/modules/BasicIMUEstimationModule.h
 */

/* This is currently a template for a IMU EstimationModule implementation */


#ifndef RAM_ESTIMATION_BASICIMUESTIMATIONMODULE_H
#define RAM_ESTIMATION_BASICIMUESTIMATIONMODULE_H

// STD Includes
#include <map>
#include <string>

// Library Includes

// Project Includes
#include "estimation/include/EstimatedState.h"
#include "estimation/include/EstimationModule.h"

#include "core/include/ConfigNode.h"
#include "core/include/AveragingFilter.h"
#include "core/include/Event.h"
#include "core/include/ReadWriteMutex.h"

#include "vehicle/include/Events.h"

#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "math/include/Events.h"

namespace ram {
namespace estimation {

typedef RawIMUData FilteredIMUData;


class BasicIMUEstimationModule : public EstimationModule
{
public:
    BasicIMUEstimationModule(core::ConfigNode config,
                             core::EventHubPtr eventHub, EstimatedStatePtr estState);
    ~BasicIMUEstimationModule(){};


    /* The IMU Estimation routine goes here.  It should store the new estimated
       state in estimatedState. */
    virtual void update(core::EventPtr event);

private:
    /* any helper functions should be prototyped here */
    std::set<std::string> imuList;
    const static int FILTER_SIZE = 10;
    /* any necessary persistent variables should be declared here */

    /** Filterd and rotated IMU data */
    std::map<std::string, FilteredIMUData*> m_filteredState;

    std::string m_magIMUName;
    std::string m_cgIMUName;

    std::map<std::string, 
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredAccelX; 
    std::map<std::string, 
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredAccelY;
    std::map<std::string,
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredAccelZ; 

    std::map<std::string, 
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredGyroX; 
    std::map<std::string, 
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredGyroY;
    std::map<std::string, 
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredGyroZ;

    std::map<std::string, 
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredMagX; 
    std::map<std::string, 
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredMagY;
    std::map<std::string, 
             core::AveragingFilter<double, FILTER_SIZE> > m_filteredMagZ; 

    core::ReadWriteMutex m_stateMutex;

};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_BASICIMUESTIMATIONMODULE_H

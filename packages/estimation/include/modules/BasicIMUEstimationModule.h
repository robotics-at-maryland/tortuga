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
#include "core/include/Event.h"
#include "core/include/ReadWriteMutex.h"

#include "vehicle/include/Events.h"

#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "math/include/Events.h"
#include "math/include/AveragingFilter.h"

namespace ram {
namespace estimation {

const static int FILTER_SIZE = 30;

typedef std::set<std::string> StringSet;

typedef RawIMUData FilteredIMUData;
typedef boost::shared_ptr<FilteredIMUData> FilteredIMUDataPtr;
typedef std::map<std::string, FilteredIMUDataPtr > FilteredStateMap;
typedef math::AveragingFilter<double, FILTER_SIZE> IMUAveragingFilter;
typedef std::map<std::string, IMUAveragingFilter > DeviceAveragingMap;

class BasicIMUEstimationModule : public EstimationModule
{
public:
    BasicIMUEstimationModule(core::ConfigNode config,
                             core::EventHubPtr eventHub,
                             EstimatedStatePtr estState);

    virtual ~BasicIMUEstimationModule(){};


    // the IMU Estimation routine goes here.  
    // it should store the new estimated state in estimatedState.
    virtual void update(core::EventPtr event);

private:
    // any helper functions should be prototyped here


    // any necessary persistent variables should be declared here
    StringSet imuList;

    // filterd and rotated IMU data
    FilteredStateMap m_filteredState;

    std::string m_magIMUName;
    std::string m_cgIMUName;

    DeviceAveragingMap m_filteredAccelX; 
    DeviceAveragingMap m_filteredAccelY;
    DeviceAveragingMap m_filteredAccelZ; 

    DeviceAveragingMap m_filteredGyroX; 
    DeviceAveragingMap m_filteredGyroY;
    DeviceAveragingMap m_filteredGyroZ;
    math::AveragingFilter<double,FILTER_SIZE> m_filteredGyroXV; 
    math::AveragingFilter<double,FILTER_SIZE> m_filteredGyroYV;
    math::AveragingFilter<double,FILTER_SIZE> m_filteredGyroZV;
    double lx;
    double ly;
    double lz;
    double m_gyroXThresh;
    double m_gyroYThresh;
    double m_gyroZThresh;

    DeviceAveragingMap m_filteredMagX; 
    DeviceAveragingMap m_filteredMagY;
    DeviceAveragingMap m_filteredMagZ; 

    core::ReadWriteMutex m_stateMutex;

};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_BASICIMUESTIMATIONMODULE_H

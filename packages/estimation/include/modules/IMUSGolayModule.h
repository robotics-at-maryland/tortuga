/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/modules/IMUSGolayModule.h
 */

/* This is currently a template for a IMU EstimationModule implementation */


#ifndef RAM_ESTIMATION_IMUSGOLAYMODULE_H
#define RAM_ESTIMATION_IMUSGOLAYMODULE_H

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
#include "math/include/SGolaySmoothingFilter.h"

namespace ram {
namespace estimation {

const static int FILTER_SIZE = 10;

typedef std::set<std::string> StringSet;

typedef RawIMUData FilteredIMUData;
typedef boost::shared_ptr<FilteredIMUData> FilteredIMUDataPtr;
typedef std::map<std::string, FilteredIMUDataPtr > FilteredStateMap;
typedef std::map<std::string, math::SGolaySmoothingFilterPtr > DeviceSGolayMap;

class IMUSGolayModule : public EstimationModule
{
public:
    IMUSGolayModule(core::ConfigNode config,
                    core::EventHubPtr eventHub,
                    EstimatedStatePtr estState);

    ~IMUSGolayModule(){};


    // the IMU Estimation routine goes here.  
    // it should store the new estimated state in estimatedState.
    virtual void update(core::EventPtr event);

private:
    // any helper functions should be prototyped here


    // any necessary persistent variables should be declared here
    StringSet imuList;

    // filterd and rotated IMU data
    FilteredStateMap m_filteredState;

    int m_degree;
    int m_windowSize;

    std::string m_magIMUName;
    std::string m_cgIMUName;

    DeviceSGolayMap m_filteredAccelX; 
    DeviceSGolayMap m_filteredAccelY;
    DeviceSGolayMap m_filteredAccelZ; 

    DeviceSGolayMap m_filteredGyroX; 
    DeviceSGolayMap m_filteredGyroY;
    DeviceSGolayMap m_filteredGyroZ;

    DeviceSGolayMap m_filteredMagX; 
    DeviceSGolayMap m_filteredMagY;
    DeviceSGolayMap m_filteredMagZ; 

    core::ReadWriteMutex m_stateMutex;

};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_IMUSGOLAYMODULE_H

/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/modules/BasicIMUEstimationModule.h
 */

/* This is currently a template for a IMU EstimationModule implementation */


#ifndef RAM_VEHICLE_ESTIMATOR_BASICIMUESTIMATIONMODULE_H
#define RAM_VEHICLE_ESTIMATOR_BASICIMUESTIMATIONMODULE_H

// Library Includes
#include <map>
#include <string>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/AveragingFilter.h"
#include "core/include/Event.h"
#include "core/include/ReadWriteMutex.h"

#include "vehicle/estimator/include/EstimatedState.h"
#include "vehicle/estimator/include/EstimationModule.h"
#include "vehicle/include/Events.h"

#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "math/include/Events.h"

namespace ram {
namespace estimator {

typedef RawIMUData FilteredIMUData;
typedef vehicle::IMUInitEventPtr IMUConfigPtr;
const static int FILTER_SIZE = 10;

class BasicIMUEstimationModule : public EstimationModule
{
public:
    BasicIMUEstimationModule(core::ConfigNode config);
    ~BasicIMUEstimationModule(){};


    /* This is called when an IMU publishes its calibration values */
    virtual void init(core::EventPtr event);

    /* The IMU Estimation routine goes here.  It should store the new estimated
       state in estimatedState. */
    virtual void update(core::EventPtr event, EstimatedStatePtr estimatedState);

private:
    /* any helper functions should be prototyped here */

    void rotateAndFilterData(const RawIMUData* newState, std::string name);
    
    static void quaternionFromIMU(double mag[3], double accel[3],
                                  double* quaternion);

    // Plan for future
    // static math::Quaternion quaternionFromIMU(math::Vector3 mag, math::Vector3 accel);
    
    static void quaternionFromRate(double* quaternionOld,
				   double angRate[3],
				   double deltaT,
				   double* quaternionNew);

    // Plan for future
    // static math::Quaternion quaternionFromRate(math::Quaternion quatOld,
    //                                            math::Vector3 angRate,
    //                                            double deltaT);

    /* any necessary persistent variables should be declared here */

    std::map<std::string, IMUConfigPtr> imuList;

    /** Filterd and rotated IMU data */
    std::map<std::string, FilteredIMUData*> m_filteredState;

    std::string m_magIMUName;
    std::string m_cgIMUName;
    math::Quaternion m_orientation;

    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredAccelX; 
    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredAccelY;
    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredAccelZ; 

    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredGyroX; 
    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredGyroY;
    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredGyroZ;

    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredMagX; 
    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredMagY;
    std::map<std::string, core::AveragingFilter<double, FILTER_SIZE> > m_filteredMagZ; 

    core::ReadWriteMutex m_orientationMutex;
    core::ReadWriteMutex m_stateMutex;

};

} // namespace estimatior
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_BASICIMUESTIMATIONMODULE_H

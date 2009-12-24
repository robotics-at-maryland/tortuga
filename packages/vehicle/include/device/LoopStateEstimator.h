/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/LoopStateEstimator.h
 */

#ifndef RAM_VEHICLE_DEVICE_LOOPSTATEESTIMATOR_06_26_2009
#define RAM_VEHICLE_DEVICE_LOOPSTATEESTIMATOR_06_26_2009

// Project Includes
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/Device.h"

#include "core/include/AveragingFilter.h"
#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"
#include "math/include/Quaternion.h"

// Must Be Included last
#include "vehicle/include/Export.h"

// Filter size
#define FILTER_SIZE 10

// Forward declaration from imuapi.h
struct _RawIMUData;
typedef _RawIMUData RawIMUData;

// Forward declaration from dvlapi.h
struct _RawDVLData;
typedef _RawDVLData RawDVLData;

namespace ram {
namespace vehicle {
namespace device {

typedef RawIMUData FilteredIMUData;

/** imu data */
typedef struct {
    core::AveragingFilter<double, FILTER_SIZE> filteredAccelX; 
    core::AveragingFilter<double, FILTER_SIZE> filteredAccelY;
    core::AveragingFilter<double, FILTER_SIZE> filteredAccelZ; 

    core::AveragingFilter<double, FILTER_SIZE> filteredGyroX; 
    core::AveragingFilter<double, FILTER_SIZE> filteredGyroY;
    core::AveragingFilter<double, FILTER_SIZE> filteredGyroZ;

    core::AveragingFilter<double, FILTER_SIZE> filteredMagX; 
    core::AveragingFilter<double, FILTER_SIZE> filteredMagY;
    core::AveragingFilter<double, FILTER_SIZE> filteredMagZ;
} IMUData;

/** dvl averaging filter */
typedef struct {
} DVLData;

/** Simple loop back estimator, ie. it doesn't esimator anything */
class RAM_EXPORT LoopStateEstimator :
        public Device, // for getName
        public IStateEstimator
        // boost::noncopyable
{
public:
    LoopStateEstimator(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr(),
                       IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~LoopStateEstimator();

    virtual void imuUpdate(IMUPacket* rawData);

    virtual void dvlUpdate(DVLPacket* rawData);
    
    virtual void depthUpdate(DepthPacket* rawData);
    
    virtual math::Quaternion getOrientation(std::string obj = "vehicle");

    virtual math::Vector3 getLinearAcceleration();

    virtual math::Vector3 getMagnetometer();

    virtual math::Vector3 getAngularRate();

    virtual math::Vector2 getVelocity(std::string obj = "vehicle");

    virtual math::Vector2 getPosition(std::string obj = "vehicle");
    
    virtual double getDepth(std::string obj = "vehicle");

    virtual bool hasObject(std::string obj);

    // Device Options
    virtual std::string getName() { return Device::getName(); }
    
    virtual void update(double timestep) {}

    virtual void setPriority(core::IUpdatable::Priority) {}

    virtual core::IUpdatable::Priority getPriority() {
        return IUpdatable::NORMAL_PRIORITY;
    }

    virtual void setAffinity(size_t) {};
    
    virtual int getAffinity() {
        return -1;
    };
    
    virtual void background(int interval) {
        //Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        //Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return true;
        //return Updatable::backgrounded();
    };

    void getFilteredIMUState(FilteredIMUData& filteredState);
    
protected:
    void rotateAndFilterData(const RawIMUData* newState,
			     IMUPacket* packet);

    static void quaternionFromIMU(double mag[3], double accel[3],
                                  double* quaternion);

    static void quaternionFromRate(double* quaternionOld,
				   double angRate[3],
				   double deltaT,
				   double* quaternionNew);

    math::Quaternion computeQuaternion(math::Vector3 mag, 
				       math::Vector3 accel,
				       math::Vector3 angRate,
				       double deltaT,
				       math::Quaternion quaternionOld,
				       const IMUPacket* packet);

    /** IMU data for the state estimator */
    IMUData m_imuData;

    /** DVL data for the state estimator */
    DVLData m_dvlData;

    /** end user data the state estimator generates */
    core::ReadWriteMutex m_orientationMutex;
    math::Quaternion m_orientation;

    core::ReadWriteMutex m_velocityMutex;
    math::Vector2 m_velocity;

    core::ReadWriteMutex m_positionMutex;
    math::Vector2 m_position;

    core::ReadWriteMutex m_depthMutex;
    double m_depth;

    /** protects access to raw state */
    core::ReadWriteMutex m_imuStateMutex;
    FilteredIMUData* m_filteredState;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_LOOPSTATEESTIMATOR_06_26_2009

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IMU.h
 */

#ifndef RAM_VEHICLE_DEVICE_IMU_06_25_2007
#define RAM_VEHICLE_DEVICE_IMU_06_25_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IIMU.h"

#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/ConfigNode.h"
#include "core/include/AveragingFilter.h"

#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"


// Forward declare structure from imuapi.h
struct _RawIMUData;
typedef _RawIMUData RawIMUData;

namespace ram {
namespace vehicle {
namespace device {

class IMU;
typedef boost::shared_ptr<IMU> IMUPtr;

const static int FILTER_SIZE = 10;

typedef RawIMUData FilteredIMUData;
    
class IMU : public IIMU,
            public Device, // for getName
            public core::Updatable // for update
            // boost::noncopyable
{
public:
    
    /** Create an IMU with the given device file */
    IMU(core::ConfigNode config,
        core::EventHubPtr eventHub = core::EventHubPtr(),
        IVehiclePtr vehicle = IVehiclePtr());

    virtual ~IMU();
    
    virtual math::Vector3 getLinearAcceleration();

    virtual math::Vector3 getMagnetometer();
    
    virtual math::Vector3 getAngularRate();

    /** Grabs the raw IMU state */
    void getRawState(RawIMUData& imuState);
    /** Grab filtered state */
    void getFilteredState(FilteredIMUData& filteredState);

    virtual std::string getName() { return Device::getName(); }
    
    /** This is called at the desired interval to read data from the IMU */
    virtual void update(double timestep);

    virtual void setPriority(core::IUpdatable::Priority priority) {
        Updatable::setPriority(priority);
    }
    
    virtual core::IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    virtual void setAffinity(size_t affinity) {
        Updatable::setAffinity(affinity);
    }
    
    virtual int getAffinity() {
        return Updatable::getAffinity();
    }
    
    virtual void background(int interval) {
        Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };
    
private:
    void rotateAndFilterData(const RawIMUData* newState);
    
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
                                       math::Quaternion quaternionOld);
    

    /** Name of the serial device file */
    std::string m_devfile;
    
    /** File descriptor for the serial device file once open */
    int m_serialFD;

    /** IMU number for the log file */
    int m_imuNum;

    /** Rotates data from the IMU to the Vehicle frame */
    double m_IMUToVehicleFrame[3][3];

    /** Bias based on vehicles local mag field */
    double m_magXBias;
    double m_magYBias;
    double m_magZBias;

    /** Bias based on flaws in the gyro */
    double m_gyroXBias;
    double m_gyroYBias;
    double m_gyroZBias;
    
    /** Magnetic Corruption Threshold based on experimental vehicle data **/
    double m_magCorruptThresh;

    /** Nominal value of magnetic vector length obtained experimentally **/
    double m_magNominalLength;
    
    /** Protects acces to public state */
    core::ReadWriteMutex m_orientationMutex;
    math::Quaternion m_orientation;
    
    /** Protects access to raw state */
    core::ReadWriteMutex m_stateMutex;
    /** The raw data read back from the IMU */
    RawIMUData* m_rawState;

    /** Filterd and rotated IMU data */
    FilteredIMUData* m_filteredState;
    
    core::AveragingFilter<double, FILTER_SIZE> m_filteredAccelX; 
    core::AveragingFilter<double, FILTER_SIZE> m_filteredAccelY;
    core::AveragingFilter<double, FILTER_SIZE> m_filteredAccelZ; 

    core::AveragingFilter<double, FILTER_SIZE> m_filteredGyroX; 
    core::AveragingFilter<double, FILTER_SIZE> m_filteredGyroY;
    core::AveragingFilter<double, FILTER_SIZE> m_filteredGyroZ;

    core::AveragingFilter<double, FILTER_SIZE> m_filteredMagX; 
    core::AveragingFilter<double, FILTER_SIZE> m_filteredMagY;
    core::AveragingFilter<double, FILTER_SIZE> m_filteredMagZ; 
};

    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IMU_06_25_2007

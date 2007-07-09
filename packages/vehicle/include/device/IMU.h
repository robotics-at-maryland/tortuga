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

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/Device.h"

#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/ConfigNode.h"
#include "core/include/AveragingFilter.h"

#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

#include "pattern/include/Subject.h"

// Forward declare structure from imuapi.h
struct _RawIMUData;
typedef _RawIMUData RawIMUData;

namespace ram {
namespace vehicle {
namespace device {

const static int FILTER_SIZE = 10;

typedef RawIMUData FilteredIMUData;
    
class IMU : public Device,          // For getName
            public core::Updatable, // for update
            public pattern::Subject // so other objects can watch
            // boost::noncopyable
{
public:
    enum UpdateEvents {
        DataUpdate
    };
    
    /** Create an IMU with the given device file */
    IMU(Vehicle* vehicle, core::ConfigNode config);

    /** Creats a new object */
    static IMUPtr construct(Vehicle* vehicle, core::ConfigNode config);

    /** Preforms a cast to the desired type */
    static IMUPtr castTo(IDevicePtr ptr);
//    static IMU* castTo(IDevice* ptr);
    
    virtual ~IMU();

    /** This is called at the desired interval to read data from the IMU */
    virtual void update(double timestep);

    
    virtual void background(int interval) {
        Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };

    math::Vector3 getLinearAcceleration();

    math::Vector3 getAngularRate();

    math::Quaternion getOrientation();
    
    /** Grabs the raw IMU state */
    void getRawState(RawIMUData& imuState);
    /** Grab filtered state */
    void getFilteredState(FilteredIMUData& filteredState);
    
private:
    void rotateAndFilterData(RawIMUData* newState);
    
    void quaternionFromIMU(double mag[3], double accel[3],
                           double* quaternion);
    
    /** Name of the serial device file */
    std::string m_devfile;
    
    /** File descriptor for the serial device file once open */
    int m_serialFD;

    /** Rotates data from the IMU to the Vehicle frame */
    double m_IMUToVehicleFrame[3][3];
    double m_localMagneticPitch;

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
    
    /** Protects access to derived state */

};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IMU_06_25_2007

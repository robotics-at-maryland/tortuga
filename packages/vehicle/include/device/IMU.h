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
#include "pattern/include/Subject.h"

// Forward declare structure from imuapi.h
struct _RawIMUData;
typedef _RawIMUData RawIMUData;

namespace ram {
namespace vehicle {
namespace device {

class IMU : public Device, // For getName
            public core::Updatable, // for update
            public pattern::Subject // so other objects can watch
            // boost::noncopyable
{
public:
    enum UpdateEvents {
        DataUpdate
    };
    
    /** Create an IMU with the given device file */
    IMU(VehiclePtr vehicle, core::ConfigNode config);

    virtual ~IMU();

    /** This is called at the desired interval to read data from the IMU */
    virtual void update(double timestep);

    /** Grabs the raw IMU state */
    void getRawState(RawIMUData& imuState);
    
private:
    /** Name of the serial device file */
    std::string m_devfile;
    
    /** File descriptor for the serial device file once open */
    int m_serialFD;

    /** Protects access to raw state */
    core::ReadWriteMutex m_rawStateMutex;
    /** The raw data read back from the IMU */
    RawIMUData* m_rawState;

    /** Protects access to derived state */
    core::ReadWriteMutex m_stateMutex;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IMU_06_25_2007

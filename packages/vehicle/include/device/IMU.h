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


namespace ram {
namespace vehicle {
namespace device {

class IMU;
typedef boost::shared_ptr<IMU> IMUPtr;
    
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
    
    /** Grabs the raw IMU state */
    void getRawState(RawIMUData& imuState);

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
    /** Name of the serial device file */
    std::string m_devfile;
    
    /** File descriptor for the serial device file once open */
    int m_serialFD;

    /** IMU number for the log file */
    int m_imuNum;

    /** Heartbeat of the IMU */
    int m_heartbeat;

    /** singleton instance of the packet */
    IMUPacket m_packet;

    /** Protects access to raw state */
    core::ReadWriteMutex m_stateMutex;
    /** The raw data read back from the IMU */
    RawIMUData* m_rawState;
};

    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IMU_06_25_2007

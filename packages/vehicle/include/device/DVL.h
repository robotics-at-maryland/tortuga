/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/device/DVL.h
 */

#ifndef RAM_VEHICLE_DEVICE_DVL_10_29_2009
#define RAM_VEHICLE_DEVICE_DVL_10_29_2009

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IVelocitySensor.h"

#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/ConfigNode.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix2.h"

// Forward declare structure from dvlapi.h
struct _RawDVLData;
typedef _RawDVLData RawDVLData;

namespace ram {
namespace vehicle {
namespace device {

class DVL;
typedef boost::shared_ptr<DVL> DVLPtr;

class DVL : public IVelocitySensor,
            public Device, // for getName
            public core::Updatable // for update
            // boost::noncopyable
{
public:
    
    /** Create an IMU with the given device file */
    DVL(core::ConfigNode config,
        core::EventHubPtr eventHub = core::EventHubPtr(),
        IVehiclePtr vehicle = IVehiclePtr());

    virtual ~DVL();
    
    virtual math::Vector3 getLocation();

    /** Grabs the raw DVL state */
    void getRawState(RawDVLData& dvlState);
    
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
    IVehiclePtr m_vehicle;
    
    /** Name of the serial device file */
    std::string m_devfile;
    
    /** File descriptor for the serial device file once open */
    int m_serialFD;

    /** DVL number for the log file */
    int m_dvlNum;
    
    /** sensor location **/
    math::Vector3 m_location;

    /** angular offset of dvl */
    double m_angOffset;

    /** rotation matrix from transducer frame to body frame */
    math::Matrix2 m_bRt;

    /** Protects access to raw state */
    core::ReadWriteMutex m_stateMutex;

    /** The raw data read back from the DVL */
    RawDVLData* m_rawState;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_DVL_10_29_2009

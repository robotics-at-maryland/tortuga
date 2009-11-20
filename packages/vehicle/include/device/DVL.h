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
#include "vehicle/include/device/IDVL.h"

#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/ConfigNode.h"
#include "core/include/AveragingFilter.h"

#include "math/include/Vector2.h"

// Forward declare structure from dvlapi.h
struct _RawDVLData;
typedef _RawDVLData RawDVLData;

namespace ram {
namespace vehicle {
namespace device {

class DVL;
typedef boost::shared_ptr<DVL> DVLPtr;

// Consult with Joe for how big he wants this filter
const static int FILTER_SIZE = 10;

typedef RawDVLData FilteredDVLData;

class DVL : public IDVL,
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

    virtual double getDepth();
    
    virtual math::Vector2 getVelocity();

    /** Grabs the raw DVL state */
    void getRawState(RawDVLData& dvlState);
    /** Grab the filtered state */
    void getFilteredState(FilteredDVLData& dvlState);
    
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

    /** DVL number for the log file */
    int m_dvlNum;

    /** Protects access to the depth */
    core::ReadWriteMutex m_depthMutex;
    double m_depth;
    
    /** Protects access to public state */
    core::ReadWriteMutex m_velocityMutex;
    math::Vector2 m_velocity;

    /** The raw data read back from the DVL */
    RawDVLData* m_rawState;

    /** Filtered and rotated IMU data */
    FilteredDVLData* m_filteredState;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_DVL_10_29_2009

/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/VisionVelocitySensor.h
 */

#ifndef RAM_VEHICLE_DEVICE_VISIONVELOCITYSENSOR_06_27_2009
#define RAM_VEHICLE_DEVICE_VISIONVELOCITYSENSOR_06_27_2009

// Project Includes
#include "vehicle/include/device/IVelocitySensor.h"
#include "vehicle/include/device/Device.h"
#include "vehicle/include/Common.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {
    
/** Implements the IVelocitySensor backed by the vision subsystem */
class RAM_EXPORT VisionVelocitySensor :
        public Device, // for getName
        public IVelocitySensor
        // boost::noncopyable
{
public:
    VisionVelocitySensor(core::ConfigNode config,
                         core::EventHubPtr eventHub = core::EventHubPtr(),
                         IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~VisionVelocitySensor();

    virtual math::Vector2 getVelocity();

    virtual math::Vector3 getLocation();

    // IDevice methods
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
    
private:
    /** Vision::EventType::VELOCITY_UPDATE event handler */
    void onVelocityUpdate(core::EventPtr event);

    /** IDepthSensor::UPDATE event handler */
    void onDepthUpdate(core::EventPtr event);

    /** Gets the conversion from pixels to meters for the X dimension */
    double getXConversionFactor();

    /** Gets the conversion from pixels to meters for the Y dimension */
    double getYConversionFactor();
    
    core::ReadWriteMutex m_mutex;
    
    /** The last velocity we calculated  */
    math::Vector2 m_velocity;

    /** The current distance from the vehicle to the bottom of the pool */
    double m_downwardDistance;
    
    /** The absolute depth of the bottom of the arena */
    double m_bottomDepth;

    /** The timestamp of the last event we recieved */
    double m_lastTimeStamp;
    
    /** VELOCITY_UPDATE event connection */
    core::EventConnectionPtr m_velocityConnection;

    /** IDepthSensor UPDATE event connection */
    core::EventConnectionPtr m_depthConnection;

};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_VISIONVELOCITYSENSOR_06_27_2009

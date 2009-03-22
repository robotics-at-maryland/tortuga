/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Vehicle.h
 */

#ifndef RAM_VEHICLE_VEHICLE_06_11_2007
#define RAM_VEHICLE_VEHICLE_06_11_2007

// STD Includes
#include <string>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/Updatable.h"

#include "vehicle/include/Common.h"
#include "vehicle/include/IVehicle.h"

namespace ram {
namespace vehicle {

class Vehicle : public IVehicle, public core::Updatable
{
public:
    Vehicle(core::ConfigNode config, 
            core::SubsystemList deps = core::SubsystemList());
        
    virtual ~Vehicle();
    
    /** Gets the device associated with the given name  <b>NOT THREAD SAFE</b>*/
    virtual device::IDevicePtr getDevice(std::string name);

    virtual std::vector<std::string> getDeviceNames();
    
    virtual double getDepth();

    math::Vector3 getLinearAcceleration();

    math::Vector3 getAngularRate();
    
    math::Quaternion getOrientation();
    
    virtual void safeThrusters();

    virtual void unsafeThrusters(); 

    virtual void dropMarker();

    virtual void fireTorpedo();
    
    virtual void applyForcesAndTorques(const math::Vector3& force,
                                       const math::Vector3& torque);

    /** This is <b>NOT</b> thread safe */
    virtual void _addDevice(device::IDevicePtr device);

    /** Sets the priority to all background devices threads */
    virtual void setPriority(core::IUpdatable::Priority priority);

    virtual core::IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    /** Sets the affinity for all background devices threads */
    virtual void setAffinity(size_t affinity);

    virtual int getAffinity() {
        return Updatable::getAffinity();
    }
         
    /** Backgrounds all devices with the given update interval */
    virtual void background(int interval);

    /** Unbackgrounds all devices */
    virtual void unbackground(bool join = false);
    
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };

    /** Currently just manually grabs depth */
    virtual void update(double timestep);

    /** Records the next 5 depths readings and sets it as the offset */
    void calibrateDepth();
    
protected:
    /** Grabs the IMU from the current list of devices */
    device::IIMUPtr getIMU();
    
    /** Grabs the depth sensor from the current list of devices*/
    device::IDepthSensorPtr getDepthSensor();

    /** Grabs the marker dropper device */
    device::IPayloadSetPtr getMarkerDropper();

    /** Grabs the torpedo launcher device */
    device::IPayloadSetPtr getTorpedoLauncher();
    
    /** Returns true if all IThrusterPtrs now contain valid thrusters */
    bool lookupThrusterDevices();
    
private:
    core::ConfigNode m_config;
    
    NameDeviceMap m_devices;

    std::string m_starboardThrusterName;
    vehicle::device::IThrusterPtr m_starboardThruster;
    std::string m_portThrusterName;
    vehicle::device::IThrusterPtr m_portThruster;
    std::string m_foreThrusterName;
    vehicle::device::IThrusterPtr m_foreThruster;
    std::string m_aftThrusterName;
    vehicle::device::IThrusterPtr m_aftThruster;
    std::string m_topThrusterName;
    vehicle::device::IThrusterPtr m_topThruster;
    std::string m_bottomThrusterName;
    vehicle::device::IThrusterPtr m_bottomThruster;

    std::string m_imuName;
    vehicle::device::IIMUPtr m_imu;

    std::string m_depthSensorName;
    vehicle::device::IDepthSensorPtr m_depthSensor;

    std::string m_markerDropperName;
    vehicle::device::IPayloadSetPtr m_markerDropper;

    std::string m_torpedoLauncherName;
    vehicle::device::IPayloadSetPtr m_torpedoLauncher;
};
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_VEHICLE_06_11_2007

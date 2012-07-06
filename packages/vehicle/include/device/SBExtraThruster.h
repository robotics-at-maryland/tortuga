/*
 * Copyright (C) 2012 Robotics at Maryland
 * Copyright (C) 2012 Gary Sullivan <gsulliva@umd.edu>
 * All rights reserved.
 *
 * Author: Gary Sullivan <gsulliva@umd.edu>
 * File:  packages/vision/include/device/SBExtraThruster.h
 */

#ifndef RAM_VEHICLE_DEVICE_SBEXTRATHRUSTER_07_05_2012
#define RAM_VEHICLE_DEVICE_SBEXTRATHRUSTER_07_05_2012

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/Common.h"
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IThruster.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace vehicle {
namespace device {

/** Implements the IThruster interface using the sensor board */
class SBExtraThruster : public Device, // for getName
                 public IThruster
                 // boost::noncopyable
{
public:
    virtual std::string getName() { return Device::getName(); }
    
    /** Create a thruster with the given address */
    SBExtraThruster(core::ConfigNode config,
             core::EventHubPtr eventHub = core::EventHubPtr(),
             IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SBExtraThruster();

    /** Sets the current thruster force of the thrusters */
    virtual void setForce(double newtons);

    /** Return the current force the thrusters are outputing in newtons */
    virtual double getForce();

    virtual double getMaxForce()
    {
        return 300;
    }
    
    virtual double getMinForce()
    {
        return 0;
    }

    virtual bool isEnabled()
    {
        return 1;
    }

    virtual void setEnabled(bool state)
    {
        return;
    }

    virtual math::Vector3 getLocation()
    {
        return math::Vector3();
    }

    virtual math::Vector3 getDirection()
    {
        return math::Vector3();
    }

    virtual double getCurrent()
    {
        return 0;
    }

    // IUpdatable Interface methods, these delegate to the communicator

    /** This will force an update on the ThrusterCommunicator */
    virtual void update(double timestep);

    virtual void setPriority(core::IUpdatable::Priority) {}

    virtual core::IUpdatable::Priority getPriority() {
        return IUpdatable::NORMAL_PRIORITY;
    }

    virtual void setAffinity(size_t) {};
    
    virtual int getAffinity() {
        return -1;
    };
    
    /** This is delegated to the ThrusterCommunicator
     *
     *  @note  It affect ALL thrusters.
     */
    virtual void background(int interval);

    /** This is delegated to the ThrusterCommunicator
     *
     *  @note  It affect ALL thrusters, so it shuts off control for all of them.
     */
    virtual void unbackground(bool join = false);

    /** This is delegated to the ThrusterCommunicator
     *
     *  @note  It will return the same value for all thrusters.
     */
    virtual bool backgrounded();
    
private:
    
    int m_address;

    core::ReadWriteMutex m_stateMutex;
    /** Current output force of motor */
    double m_force;

    /** Whether or not the thruster accepts commands */
    bool m_enabled;
    
    /** The sensor board which provides access to the hardware */
    SensorBoardPtr m_sensorBoard;

    /** MOTORCURRENT_UPDATE event connection */
    core::EventConnectionPtr m_connection;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SBEXTRATHRUSTER_07_05_2012

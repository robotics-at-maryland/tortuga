/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/SBThruster.h
 */

#ifndef RAM_VEHICLE_DEVICE_SBTHRUSTER_06_25_2007
#define RAM_VEHICLE_DEVICE_SBTHRUSTER_06_25_2007

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
class SBThruster : public Device, // for getName
                 public IThruster
                 // boost::noncopyable
{
public:
    virtual std::string getName() { return Device::getName(); }
    
    /** Create a thruster with the given address */
    SBThruster(core::ConfigNode config,
             core::EventHubPtr eventHub = core::EventHubPtr(),
             IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SBThruster();

    /** Sets the current thruster force of the thrusters */
    virtual void setForce(double newtons);

    /** Return the current force the thrusters are outputing in newtons */
    virtual double getForce();

    virtual double getMaxForce();
    
    virtual double getMinForce();

    virtual bool isEnabled();

    virtual void setEnabled(bool state);

    virtual math::Vector3 getLocation();

    virtual double getCurrent();
    
    /** Gets the current motor count */
    int getMotorCount();


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
    /** Handles THRUSTER_UPDATE events */
    void onThrusterUpdate(core::EventPtr event);

    void doEnabledEvents(bool current, bool newState);
    
    int m_address;

    /** Experimentally determined calibration factor */
    double m_calibrationFactor;

    core::ReadWriteMutex m_stateMutex;
    /** Current output force of motor */
    double m_force;
    
    /** Current motor count */
    int m_motorCount;

    /** Directional bias for motor */
    int m_direction;

    /** The offset from axis perpendicular to axis of induced rotation */
    double m_offset;

    /** The location of the thruster with respect to the center of mass */
    math::Vector3 m_location;

    /** Current amperage draw of the thruster */
    double m_current;

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

#endif // RAM_VEHICLE_DEVICE_SBTHRUSTER_06_25_2007

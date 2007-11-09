/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/Thruster.h
 */

#ifndef RAM_VEHICLE_DEVICE_THRUSTER_06_25_2007
#define RAM_VEHICLE_DEVICE_THRUSTER_06_25_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IThruster.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace vehicle {
namespace device {

class Thruster : public Device, // for getName
                 public IThruster
                 // boost::noncopyable
{
public:
    enum UpdateEvents {
        FORCE_UPDATE
    };

    virtual std::string getName() { return Device::getName(); }
    
    /** Create a thruster with the given address */
    Thruster(core::ConfigNode config);
    
    virtual ~Thruster();

    /** Creats a new object */
    static ThrusterPtr construct(core::ConfigNode config);

    /** Preforms a cast to the desired type */
    static ThrusterPtr castTo(IDevicePtr ptr);
//    static Thruster* castTo(IDevice* ptr);
    
    /** Sets the current thruster force of the thrusters */
    void setForce(double newtons);

    /** Return the current force the thrusters are outputing in newtons */
    double getForce();

    /** Gets the current motor count */
    int getMotorCount();
    
    /** Kills all thruster power to ALL thrusters */
    //void kill();

    // IUpdatable Interface methods, these delegate to the communicator

    /** This will force an update on the ThrusterCommunicator */
    virtual void update(double timestep);

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

    /** Returns the address of the thruster */
    int getAddress() { return m_address; }
    
private:
    int m_address;

    /** Experimentally determined calibration factor */
    double m_calibrationFactor;

    core::ReadWriteMutex m_forceMutex;
    /** Current output force of motor */
    double m_force;
    
    /** Current motor count */
    int m_motorCount;

    /** Directional bias for motor */
    int m_direction;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_THRUSTER_06_25_2007

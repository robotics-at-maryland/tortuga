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

namespace ram {
namespace vehicle {
namespace device {

class Thruster : public Device // boost::noncopyable
{
public:
    /** Reset thruster, clear registers and sets power to zero */
    static const std::string SOFT_RESET; // ("Y");
    /** Sets the thruster output force */
    static const std::string SET_FORCE; // ("C");
    
    /** Create a thruster with the given address */
    Thruster(std::string name, std::string address,
             double calibrationFactor);

    virtual ~Thruster();
    
    /** Sets the current thruster force of the thrusters */
    void setForce(double newtons);

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
    
private:
    std::string m_address;
    double m_calibrationFactor;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_THRUSTER_06_25_2007

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/BWPDController.h
 */

#ifndef RAM_CONTROL_BWDCONTROLER_07_07_2007
#define RAM_CONTROL_BWDCONTROLER_07_07_2007

// Project Includes
#include "control/include/Common.h"
#include "control/include/IController.h"
#include "vehicle/include/Common.h"
#include "pattern/include/Subject.h"
#include "core/include/ConfigNode.h"
#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace control {

/** Implements a PD Controller, based on a paper by Bong Wie  */
class BWPDController : public IController,
                       public core::Updatable
{
public:
    enum UPDATE_EVENTS {
        SPEED_UPDATE,
        HEADING_UPDATE,
        DEPTH_UPDATE

    };
    BWPDController(vehicle::Vehicle* vehicle, core::ConfigNode config);

    virtual ~BWPDController();
    
    /** Set the current speed, clamped between -5 and 5 */
    virtual void setSpeed(int speed);

    /** Sets the current heading in degrees off north */
    virtual void setHeading(double degrees);

    /** Sets the current depth of the sub in meters */
    virtual void setDepth(double depth);

    /** Rolls the desired quaternion by a desired angle in degress (ugh!)
	a positive angle makes the vehicle barrel roll clockwise as seen
	by the fore camera
    */
    virtual void rollVehicle(double degrees);

    /** Pitch the desired quaternion by a desired angle in degrees (ugh!)
	a positive angle makes the vehicle spin down as seen by the fore camera
    */
    virtual void pitchVehicle(double degrees);
    
    /** Yaws the desired quaternion by a desired angle in degrees (ugh!)
	a positive angle makes the vehicle spin left as seen by the fore camera
    */
    virtual void yawVehicle(double degrees);

    virtual bool isOriented();

    virtual int getSpeed();

    virtual double getHeading();

    virtual double getDepth();

    virtual void background(int interval) {
        Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };

    /** Grabs latest vehicle state, runs the controller, the commands the
        thrusters */
    virtual void update(double timestep);
    
private:
    /** Combines the given force and torque into motor forces the applies them

        @note   All force in <b>NEWTONS</b>
        
        @param  translationalForces  3D Array
        @param  rotationalForces   3D Array
     */
    void applyForcesAndTorques(double* translationalForces,
                               double* rotationalForces);
    
    /** Out Vehicle */
    vehicle::Vehicle* m_vehicle;
    
    /** Contains settings for the controller */
    core::ConfigNode m_config;
    
    /** Syncs asscess to the desired state */
    core::ReadWriteMutex m_desiredStateMutex;
    
    /** State the controller is trying to reach */
    DesiredState* m_desiredState;

    /** State of the vehicle as the controller sees it */
    MeasuredState* m_measuredState;

    /** Internal State of the controller */
    ControllerState* m_controllerState;

    /** Values needed for thruster combination */
    double m_rStarboard;
    double m_rPort;
    double m_rFore;
    double m_rAft;

    vehicle::device::ThrusterPtr m_starboardThruster;
    vehicle::device::ThrusterPtr m_portThruster;
    vehicle::device::ThrusterPtr m_foreThruster;
    vehicle::device::ThrusterPtr m_aftThruster;

    vehicle::device::IMUPtr m_imu;

    /* Pitch hack gain */
    double m_hackedPitchGain;
    double m_hackedYawGain;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_BWDCONTROLER_07_07_2007

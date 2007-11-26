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

// STD Includes
#include <fstream>

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

/** Tolerance for at Depth (1 foot in meters) */
static const double DEPTH_TOLERANCE = 0.3048;

static const double ORIENTATION_THRESHOLD = 0.015;
    
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
    BWPDController(vehicle::IVehiclePtr vehicle, core::ConfigNode config);

    BWPDController(core::ConfigNode config,
                   core::SubsystemList deps = core::SubsystemList());

    virtual ~BWPDController();
    
    /** @copydoc IController::setSpeed() */
    virtual void setSpeed(int speed);

    /** Sets the current heading in degrees off north */
    virtual void setHeading(double degrees);

    /** @copydoc IController::setDepth() */
    virtual void setDepth(double depth);

    /** @copydoc IController::getSpeed() */
    virtual int getSpeed();

    virtual double getHeading();

    /** @copydoc IController::getDepth() */
    virtual double getDepth();

    /** Rolls the desired quaternion by a desired angle in degress (ugh!)
	a positive angle makes the vehicle barrel roll clockwise as seen
	by the fore camera
    */
    virtual void rollVehicle(double degrees);

    /** Pitch the desired quaternion by a desired angle in degrees (ugh!)
	a positive angle makes the vehicle spin down as seen by the fore camera
    */
    virtual void pitchVehicle(double degrees);

    /** @copydoc IController::yawVehicle() */
    virtual void yawVehicle(double degrees);
    
    /** @copydoc IController::isOriented() */
    virtual bool isOriented();

    /** @copydoc IController::atDepth() */
    virtual bool atDepth();
    
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
    void init(core::ConfigNode config);
    
    /** Out Vehicle */
    vehicle::IVehiclePtr m_vehicle;
    
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

    std::ofstream m_logfile;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_BWDCONTROLER_07_07_2007


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

#include "core/include/ConfigNode.h"
#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "control/include/Export.h"

#include "math/include/Vector3.h"

namespace ram {
namespace control {

/** Tolerance for at Depth (1 foot in meters) */
static const double DEPTH_TOLERANCE = 0.5;

/** About a 2.5 degree tolerance */
static const double ORIENTATION_THRESHOLD = 0.03;
    
/** Implements a PD Controller, based on a paper by Bong Wie  */
class RAM_EXPORT BWPDController : public IController,
                                  public core::Updatable
{
public:
    BWPDController(vehicle::IVehiclePtr vehicle, core::ConfigNode config);

    BWPDController(core::ConfigNode config,
                   core::SubsystemList deps = core::SubsystemList());

    virtual ~BWPDController();
    
    /** @copydoc IController::setSpeed() */
    /** NOT IMPLEMENTED */
    virtual void setVelocity(math::Vector2 velocity);

    /** NOT IMPLEMENTED */
    virtual math::Vector2 getVelocity();
    
    virtual void setSpeed(double speed);

    virtual void setSidewaysSpeed(double speed);
    
    /** Sets the current heading in degrees off north */
    virtual void setHeading(double degrees);

    /** @copydoc IController::setDepth() */
    virtual void setDepth(double depth);

    /** @copydoc IController::getSpeed() */
    virtual double getSpeed();

    virtual double getSidewaysSpeed();
    
    virtual double getHeading();

    /** @copydoc IController::getDepth() */
    virtual double getDepth();
    
    /** Grab current estimated depth*/
    virtual double getEstimatedDepth();
    
    /** Grab current estimated depth velocity (depthDot)*/
    virtual double getEstimatedDepthDot();

    /** Rolls the desired quaternion by a desired angle in degrees (ugh!)
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

    /** Gets the current desired orientation */
    virtual math::Quaternion getDesiredOrientation();
    
    /** Sets the current desired orientation */
    virtual void setDesiredOrientation(math::Quaternion);
    
    /** @copydoc IController::atOrientation() */
    virtual bool atOrientation();

    /** @copydoc IController::atDepth() */
    virtual bool atDepth();

    virtual void holdCurrentDepth();

    /** loads current orientation into desired (fixes offset in roll and pitch)*/
    virtual void holdCurrentHeading();

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

    /** Called at a fixed rate.  Grabs latest vehicle state, runs the controller, the commands the
        thrusters */
    virtual void update(double timestep);
    
    virtual void setBuoyantTorqueCorrection(double x, double y, double z);
    
private:
    void init(core::ConfigNode config);

    void publishAtDepth();

    void publishAtOrientation();
    
    /** Used to maintain state, so we don't issue continuous at depth updates */
    bool m_atDepth;

    /** True when we are at the desired orientation */
    bool m_atOrientation;
    
    /** When we are within this limit we send off the at depth event */
    double m_depthThreshold;

    /** When we are within this limit we send of the at orientation event */
    double m_orientationThreshold;
    
    /** Out Vehicle */
    vehicle::IVehiclePtr m_vehicle;
    
    /** Contains settings for the controller */
    core::ConfigNode m_config;
    
    /** Syncs asscess to the desired state */
    core::ReadWriteMutex m_desiredEstimatedStateMutex;
    
    /** State the controller is trying to reach */
    DesiredState* m_desiredState;

    /** State of the vehicle as the controller sees it */
    MeasuredState* m_measuredState;
    
    /** Estimated state of the vehicle */
    EstimatedState* m_estimatedState;
    
    /** Internal State of the controller */
    ControllerState* m_controllerState;
    
    math::Vector3 m_buoyantTorqueCorrection;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_BWDCONTROLER_07_07_2007


/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/CombineController.h
 */

#ifndef RAM_CONTROL_COMBINECONTROLLER_08_07_2008
#define RAM_CONTROL_COMBINECONTROLLER_08_07_2008

// Project Includes
#include "control/include/IController.h"

#include "vehicle/include/Common.h"

#include "core/include/ConfigNode.h"
#include "core/include/Updatable.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {
    
/** A class whichs allows easy combine of fundamental controllers
 *
 *  This class easily lets you change out just the rotational, or just the
 *  depth just the in plane controller.  Which allow for much easier over
 *  all experimentation with controllers.
 */
class RAM_EXPORT CombineController : public IController,
                                     public core::Updatable
{
public:
    CombineController(vehicle::IVehiclePtr vehicle, core::ConfigNode config);

    CombineController(core::ConfigNode config,
                      core::SubsystemList deps = core::SubsystemList());

    virtual ~CombineController();

    // In plane controller setters
    virtual void setSpeed(double speed);
    virtual void setSidewaysSpeed(double speed);

    // Depth controller setters
    virtual void setDepth(double depth);

    // In plane controller getters
    virtual double getSpeed();
    virtual double getSidewaysSpeed();
    
    // Depth controller getters
    virtual double getDepth();
    virtual double getEstimatedDepth();
    virtual double getEstimatedDepthDot();

    // Rotational controller methods
    virtual void yawVehicle(double degrees);
    virtual void pitchVehicle(double degrees);
    virtual void rollVehicle(double degrees);
    virtual math::Quaternion getDesiredOrientation();
    virtual void setDesiredOrientation(math::Quaternion);

    // Misc methods
    virtual bool atDepth();
    virtual bool atOrientation();
    virtual void holdCurrentHeading();
    virtual void setBuoyantTorqueCorrection(double x, double y, double z){};

    // Updatable methods
    virtual void update(double timeStep);
    
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
//protected:
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_COMBINECONTROLLER_08_07_2008

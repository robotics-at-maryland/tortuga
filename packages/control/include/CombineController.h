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
 *  depth just the in plane controller.  Which allows for much easier over
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

    // Translational controller methods
    virtual void setSpeed(double speed);
    virtual void setSidewaysSpeed(double speed);
    virtual double getSpeed();
    virtual double getSidewaysSpeed();
    
    // Depth controller methods
    virtual void setDepth(double depth);
    virtual double getDepth();
    virtual double getEstimatedDepth();
    virtual double getEstimatedDepthDot();

    // Rotational controller methods
    virtual void yawVehicle(double degrees);
    virtual void pitchVehicle(double degrees);
    virtual void rollVehicle(double degrees);
    virtual math::Quaternion getDesiredOrientation();
    virtual void setDesiredOrientation(math::Quaternion);
    virtual bool atOrientation();
    virtual void holdCurrentHeading();

    // Misc methods
    virtual bool atDepth();

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
    
private:
    vehicle::IVehiclePtr m_vehicle;
    ITranslationalControllerImpPtr m_transController;
    IDepthControllerImpPtr m_depthController;
    IRotationalControllerImpPtr m_rotController;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_COMBINECONTROLLER_08_07_2008

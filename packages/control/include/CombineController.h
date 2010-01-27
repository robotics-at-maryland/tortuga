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
#include "control/include/ControllerBase.h"
#include "control/include/Common.h"

#include "vehicle/include/Common.h"

#include "core/include/ConfigNode.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

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
class RAM_EXPORT CombineController : public ControllerBase
{
public:
    /** Construct the controller with the given vehicle */
    CombineController(vehicle::IVehiclePtr vehicle, core::ConfigNode config);
    
    /** The controller finds its vehicle from the given list of subsystems */
    CombineController(core::ConfigNode config,
                      core::SubsystemList deps = core::SubsystemList());

    virtual ~CombineController();
    
    /**
     * \defgroup In plane controller methods
     */
    /* @{ */
    virtual void setVelocity(math::Vector2 velocity);
    virtual math::Vector2 getVelocity();
    virtual void setSpeed(double speed);
    virtual void setSidewaysSpeed(double speed);
    virtual double getSpeed();
    virtual double getSidewaysSpeed();
    virtual void holdCurrentPosition();
    /* @{ */
    
    /**
     * \defgroup Depth controller methods
     */
    /* @{ */
    virtual void setDepth(double depth);
    virtual double getDepth();
    virtual double getEstimatedDepth();
    virtual double getEstimatedDepthDot();
    virtual bool atDepth();
    virtual void holdCurrentDepth();
    /* @{ */
    
    /**
     * \defgroup Rotational controller methods
     */
    /* @{ */
    virtual void yawVehicle(double degrees);
    virtual void pitchVehicle(double degrees);
    virtual void rollVehicle(double degrees);
    virtual math::Quaternion getDesiredOrientation();
    virtual void setDesiredOrientation(math::Quaternion);
    virtual bool atOrientation();
    virtual void holdCurrentHeading();
    /* @{ */

    ITranslationalControllerPtr getTranslationalController();
    
    IDepthControllerPtr getDepthController();
    
    IRotationalControllerPtr getRotationalController();

protected:
    virtual void doUpdate(const double& timestep,
                          const math::Vector3& linearAcceleration,
                          const math::Quaternion& orientation,
                          const math::Vector3& angularRate,
                          const double& depth,
                          const math::Vector2& position,
                          const math::Vector2& velocity,
                          math::Vector3& translationalForceOut,
                          math::Vector3& rotationalTorqueOut);

    
private:
    /** Does all initialzation based on the configuration settings */
    void init(core::ConfigNode config);

    /** Controller which handles the inplane vehicle motion */
    ITranslationalControllerImpPtr m_transController;
    
    /** Controller which holds the vehicle at the desired depth */
    IDepthControllerImpPtr m_depthController;
    
    /** Controller which holds the vehicle at the desired attitude */
    IRotationalControllerImpPtr m_rotController;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_COMBINECONTROLLER_08_07_2008

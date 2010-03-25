/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/AggregateController.h
 */

#ifndef RAM_CONTROL_AGGREGATECONTROLLER_3_2010
#define RAM_CONTROL_AGGREGATECONTROLLER_3_2010

// Project Includes
#include "control/include/Common.h"
#include "control/include/IController.h"
#include "control/include/ControllerBase.h"

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
//static const double DEPTH_TOLERANCE = 0.5;

/** About a 2.5 degree tolerance */
//static const double ORIENTATION_THRESHOLD = 0.03;
    

class RAM_EXPORT AggregateController : public ControllerBase
{
public:
    AggregateController(vehicle::IVehiclePtr vehicle, core::ConfigNode config);

    AggregateController(core::ConfigNode config,
                   core::SubsystemList deps = core::SubsystemList());

    virtual ~AggregateController();

    virtual void setVelocity(math::Vector2 velocity);
    virtual math::Vector2 getVelocity();
    virtual void setSpeed(double speed);
    virtual void setSidewaysSpeed(double speed);
    virtual double getSpeed();
    virtual double getSidewaysSpeed();
    virtual void holdCurrentPosition();
    virtual void setDesiredVelocity(math::Vector2 velocity);
    virtual void setDesiredPosition(math::Vector2 position);
    virtual void setDesiredPositionAndVelocity(math::Vector2 position, 
					       math::Vector2 velocity);
    virtual math::Vector2 getDesiredVelocity();
    virtual math::Vector2 getDesiredPosition();
    virtual bool atPosition();
    virtual bool atVelocity();

    virtual void setDepth(double depth);
    virtual double getDepth();
    virtual double getEstimatedDepth();
    virtual double getEstimatedDepthDot();
    virtual bool atDepth();
    virtual void holdCurrentDepth();

    virtual void rollVehicle(double degrees);
    virtual void pitchVehicle(double degrees);
    virtual void yawVehicle(double degrees);
    virtual math::Quaternion getDesiredOrientation();
    virtual void setDesiredOrientation(math::Quaternion);
    virtual bool atOrientation();
    virtual void holdCurrentHeading();


    ITranslationalControllerPtr getTranslationalController();
    
    IDepthControllerPtr getDepthController();
    
    IRotationalControllerPtr getRotationalController();

 protected:
    void doUpdate(const double& timestep,
                          const math::Vector3& linearAcceleration,
                          const math::Quaternion& orientation,
                          const math::Vector3& angularRate,
                          const double& depth,
                          const math::Vector2& position,
                          const math::Vector2& velocity,
                          math::Vector3& translationalForceOut,
                          math::Vector3& rotationalTorqueOut);

    
private:
    void init(core::ConfigNode config);

    ITranslationalControllerImpPtr m_transController;
    IDepthControllerImpPtr m_depthController;
    IRotationalControllerImpPtr m_rotController;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_AGGREGATECONTROLLER_3_2010


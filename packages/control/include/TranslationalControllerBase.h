/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/ITranslationalController.h
 */

#ifndef RAM_CONTROL_TRANSLATIONALCONTROLLERBASE_03_15_2009
#define RAM_CONTROL_TRANSLATIONALCONTROLLERBASE_03_15_2009

// Project Includes
#include "control/include/ITranslationalController.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/** Defines the interface for controler which controls in plane motion */
class RAM_EXPORT TranslationalControllerBase :
        public ITranslationalControllerImp
{
public:
    /** The exact type of translation control we are undergoing */
    struct ControlMode
    {
        enum ModeType {
            OPEN_LOOP,
            VELOCITY,
            POSITION,
            POSITIONANDVELOCITY
        };
    };
    
    TranslationalControllerBase(core::ConfigNode config);
    
    virtual ~TranslationalControllerBase() {}

    virtual void setVelocity(math::Vector2 velocity);

    virtual math::Vector2 getVelocity();

    virtual void setSpeed(double speed);

    virtual void setSidewaysSpeed(double speed);

    virtual double getSpeed();

    virtual double getSidewaysSpeed();

    virtual void holdCurrentPosition();
   
    virtual void setDesiredVelocity(math::Vector2 velocity);
    virtual void setDesiredPosition(math::Vector2 position);
    virtual void setDesiredPositionAndVelocity(math::Vector2 position, math::Vector2 velocity);
    virtual math::Vector2 getDesiredVelocity();
    virtual math::Vector2 getDesiredPosition();

    virtual math::Vector3 translationalUpdate(double timestep,
                                              math::Vector3 linearAcceleration,
                                              math::Quaternion orientation,
                                              math::Vector2 position,
                                              math::Vector2 velocity);

    virtual bool atPosition();
    virtual bool atVelocity();

    virtual ControlMode::ModeType getMode();
    

 protected:

    /** Syncs asscess to the shared state */
    core::ReadWriteMutex m_stateMutex;
    
    double m_desiredSpeed;
    double m_desiredSidewaysSpeed;
    math::Vector2 m_desiredVelocity;
    math::Vector2 m_desiredPosition;

    math::Vector2 m_currentVelocity;
    math::Vector2 m_currentPosition;

    double m_positionThreshold;
    double m_velocityThreshold;

    /** What type of translation control we are doing */
    ControlMode::ModeType m_controlMode;
private:
    /** Does all initialzation based on the configuration settings */
    void init(core::ConfigNode config);
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_TRANSLATIONALCONTROLLERBASE_03_15_2009
